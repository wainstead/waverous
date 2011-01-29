/******************************************************************************
  Copyright (c) 1992, 1995, 1996 Xerox Corporation.  All rights reserved.
  Portions of this code were written by Stephen White, aka ghond.
  Use and copying of this software and preparation of derivative works based
  upon this software are permitted.  Any distribution of this software or
  derivative works must comply with all applicable United States export
  control laws.  This software is made available AS IS, and Xerox Corporation
  makes no warranty about the software, its performance or its conformity to
  any specification.  Any person obtaining a copy of this software is requested
  to send their name and post office or electronic mail address to:
    Pavel Curtis
    Xerox PARC
    3333 Coyote Hill Rd.
    Palo Alto, CA 94304
    Pavel@Xerox.Com
 *****************************************************************************/

#include <sys/time.h>
#include <math.h>

#include "my-string.h"
#include "my-time.h"

#include "config.h"
#include "db.h"
#include "db_io.h"
#include "decompile.h"
#include "eval_env.h"
#include "eval_vm.h"
#include "exceptions.h"
#include "execute.h"
#include "functions.h"
#include "list.h"
#include "log.h"
#include "match.h"
#include "numbers.h"
#include "options.h"
#include "parse_cmd.h"
#include "parser.h"
#include "random.h"
#include "server.h"
#include "storage.h"
#include "streams.h"
#include "structures.h"
#include "tasks.h"
#include "utils.h"
#include "verbs.h"
#include "version.h"

typedef enum {
    /* Input Tasks */
    TASK_INBAND,		/* vanilla in-band */
    TASK_OOB,			/* out-of-band unless disable_oob */
    TASK_QUOTED,		/* in-band; needs unquote unless disable-oob */
    TASK_BINARY,		/* in-band; binary mode string */
    /* Background Tasks */
    TASK_FORKED,
    TASK_SUSPENDED
} task_kind;

typedef struct forked_task {
    int id;
    Program *program;
    activation a;
    Var *rt_env;
    int f_index;
    struct timeval start_time;
} forked_task;

typedef struct suspended_task {
    vm the_vm;
    struct timeval start_time;
    Var value;
} suspended_task;

typedef struct {
    char *string;
    int length;
    struct task *next_itail;	/* see tqueue.first_itail */
} input_task;

typedef struct task {
    struct task *next;
    task_kind kind;
    union {
	input_task input;
	forked_task forked;
	suspended_task suspended;
    } t;
} task;

enum icmd_flag {
    /* fix icmd_index() if you change any of the following numbers: */
    ICMD_SUFFIX = 1,
    ICMD_OUTPUTSUFFIX = 2,
    ICMD_OUTPUTPREFIX = 3,
    ICMD_PREFIX = 4,
    ICMD_PROGRAM = 5,		/* .program */
    /* mask */
    ICMD_ALL_CMDS = ((1 << (ICMD_PROGRAM + 1)) - 2)
};

typedef struct tqueue {
    /*
     * A task queue can be in one of four possible states, depending upon the
     * values of its `player' and `connected' slots.
     *
     * When people first connect to the MOO, they are assigned negative player
     * object numbers.  Once they log in (either by creating a new player
     * object or by connecting to an existing one), the connection is changed
     * to refer to the appropriate, non-negative, player number.
     *
     * Input tasks for logged-in players (with non-negative `player' slots) are
     * interpreted as normal MOO commands.  Those for anonymous players (with
     * negative `player' slots) are treated specially: they are passed to a
     * particular verb on the system object.  If that verb returns a valid
     * player object number, then that is used as the new player number for the
     * connection.  (Note that none of this applies to tasks that are being
     * read() or that are being handled as out-of-band commands.)
     *
     * The `connected' field is true iff this queue has an associated open
     * network connection.  Queues without such connections may nonetheless
     * contain input tasks, either typed ahead before disconnection or else
     * entered via the `force_input()' function.
     *
     * If an unconnected queue becomes empty, it is destroyed.
     */
    struct tqueue *next, **prev;	/* prev only valid on idle_tqueues */
    Objid player;
    Objid handler;
    int connected;
    task *first_input, **last_input;
    task *first_itail, **last_itail;
    /* The input queue alternates between contiguous sequences of TASK_OOBs
     * and sequences of non-TASK_OOBs; the "itail queue" is the queue of all
     * sequence-ending tasks threaded along the next_itail pointers.
     * first_itail is null iff first_input is null
     * When the queue is nonempty,
     *   last_itail points to the penultimate .next_itail pointer slot
     *   unlike last_input which always points to the final (null)
     *   .next pointer slot
     * For tasks not at the end of a sequence,
     *   the next_itail field is ignored and may be garbage.
     */
    int total_input_length;
    int last_input_task_id;
    int input_suspended;

    task *first_bg, **last_bg;
    int usage;			/* a kind of inverted priority */
    int num_bg_tasks;		/* in either here or waiting_tasks */
    char *output_prefix, *output_suffix;
    const char *flush_cmd;
    Stream *program_stream;
    Objid program_object;
    const char *program_verb;

    /* booleans */
    char hold_input;		/* input tasks must wait for read() */
    char disable_oob;		/* treat all input lines as inband */
    char reading;		/* some task is blocked on read() */
    char icmds;			/* which of .program/PREFIX/... are enabled */
    vm reading_vm;
} tqueue;

typedef struct ext_queue {
    struct ext_queue *next;
    task_enumerator enumerator;
} ext_queue;

#define INPUT_HIWAT	MAX_QUEUED_INPUT
#define INPUT_LOWAT	(INPUT_HIWAT / 2)

#define NO_USAGE	-1

int current_task_id;
static tqueue *idle_tqueues = 0, *active_tqueues = 0;
static task *waiting_tasks = 0;	/* forked and suspended tasks */
static ext_queue *external_queues = 0;

#define GET_START_TIME(ttt) \
    (ttt->kind == TASK_FORKED \
     ? ttt->t.forked.start_time \
     : ttt->t.suspended.start_time)

static void
double_to_timeval(double d, struct timeval *tv)
{
    double secs, usecs, frac;

    frac = modf(d, &secs);
    usecs = ((double) 1000000) * frac;

    tv->tv_sec = (long) secs;
    tv->tv_usec = (long) usecs;

    return;
}

static double
timeval_to_double(struct timeval *tv)
{
    double r;

    r = tv->tv_sec;
    r += tv->tv_usec / ((double) 1000000);

    return r;
}

static int
timeval_lt(struct timeval *tv1, struct timeval *tv2)
/* Returns true if tv1 is less than tv2. If either arg is NULL, time 0 is assumed. */
{
    long secs1, usecs1;
    long secs2, usecs2;

    if (tv1 == NULL) {
	secs1 = 0;
	usecs1 = 0;
    } else {
	secs1 = tv1->tv_sec;
	usecs1 = tv1->tv_usec;
    }

    if (tv2 == NULL) {
	secs2 = 0;
	usecs2 = 0;
    } else {
	secs2 = tv2->tv_sec;
	usecs2 = tv2->tv_usec;
    }

    if (secs1 < secs2)
	return 1;

    if ((secs1 == secs2) && (usecs1 < usecs2))
	return 1;

    return 0;
}

static void
timeval_add(struct timeval *result, struct timeval *tv1,
	    struct timeval *tv2)
/* Adds *tv1 and *tv2 and writes the result to *result. */
/* Neither *tv1 nor *tv2 should have negative fields. */
/* It's safe to have (result == tv1) || (result == tv2) */
{
    struct timeval sum;

    sum.tv_sec = tv1->tv_sec;
    sum.tv_sec += tv2->tv_sec;

    sum.tv_usec = tv1->tv_usec;
    sum.tv_usec += tv2->tv_usec;

    if (sum.tv_usec >= 1000000) {
	/* Microsecond carry */
	sum.tv_sec += sum.tv_usec / 1000000;
	sum.tv_usec %= 1000000;
    }

    if ((sum.tv_sec < tv1->tv_sec) || (sum.tv_sec < tv2->tv_sec)) {
	/* Seconds overflow */
	sum.tv_sec = INT_MAX;
    }

    *result = sum;
}

static void
timeval_sub(struct timeval *result, struct timeval *tv1,
	    struct timeval *tv2)
/* Subtracts *tv2 from *tv1 and stores the result in *result. */
/* It's safe to have (result == tv1) || (result == tv2) */
{
    struct timeval diff;

    diff.tv_sec = tv1->tv_sec;
    diff.tv_usec = tv1->tv_usec;

    diff.tv_sec -= tv2->tv_sec;
    diff.tv_usec -= tv2->tv_usec;

    if (diff.tv_usec < 0) {
	/* Microsecond borrow */
	diff.tv_sec -= ((-diff.tv_usec) / 1000000) + 1;
	diff.tv_usec = 1000000 - ((-diff.tv_usec) % 1000000);
    }

    *result = diff;
}

/* 
 *  ICMD_FOR_EACH(DEFINE,verb)
 *   expands to a table of intrinsic commands,
 *   each entry of the form
 *
 *  DEFINE(ICMD_NAME,<name>,<matcher>)  where
 *      ICMD_NAME == enumeration constant name to use
 *      <name>    == full verbname
 *      <matcher>(verb) -> true iff verb matches <name>
 */
#define __IDLM(DEFINE,DELIMITER,verb)			\
      DEFINE(ICMD_##DELIMITER,  DELIMITER,		\
	     (strcmp(verb, #DELIMITER) == 0))		\

#define ICMD_FOR_EACH(DEFINE,verb)			\
      DEFINE(ICMD_PROGRAM, .program,			\
	     (verbcasecmp(".pr*ogram", (verb))))	\
      __IDLM(DEFINE,PREFIX,      (verb))		\
      __IDLM(DEFINE,SUFFIX,      (verb))		\
      __IDLM(DEFINE,OUTPUTPREFIX,(verb))		\
      __IDLM(DEFINE,OUTPUTSUFFIX,(verb))		\

static int
icmd_index(const char *verb)
{
    /* evil, poor-man's minimal perfect hash */
    int len = strlen(verb);
    char c2 = len > 2 ? verb[2] : 0;
    char c8 = len > 8 ? verb[8] : 0;
    switch (((c2 & 7) ^ 6) + !(c8 & 2)) {
    default:
	break;
#define _ICMD_IX(ICMD_PREFIX,_,MATCH)		\
	case ICMD_PREFIX:			\
	    if (MATCH) return ICMD_PREFIX;	\
	    break;				\

	ICMD_FOR_EACH(_ICMD_IX, verb);
    }
    return 0;
}

#undef _ICMD_IX

static Var
icmd_list(int icmd_flags)
{
    Var s;
    Var list = new_list(0);
    s.type = (var_type) TYPE_STR;
#define _ICMD_MKSTR(ICMD_PREFIX,PREFIX,_)	\
	if (icmd_flags & (1<<ICMD_PREFIX)) {	\
	    s.v.str = str_dup(#PREFIX);		\
	    list = listappend(list, s);		\
	}					\

    ICMD_FOR_EACH(_ICMD_MKSTR, @);
    return list;
}

#undef _ICMD_MKSTR

static int
icmd_set_flags(tqueue * tq, Var list)
{
    int i;
    int newflags;
    if (list.type == TYPE_INT) {
	newflags = is_true(list) ? ICMD_ALL_CMDS : 0;
    } else if (list.type != TYPE_LIST)
	return 0;
    else {
	newflags = 0;
	for (i = 1; i <= list.v.list[0].v.num; ++i) {
	    int icmd;
	    if (list.v.list[i].type != TYPE_STR)
		return 0;
	    icmd = icmd_index(list.v.list[i].v.str);
	    if (!icmd)
		return 0;
	    newflags |= (1 << icmd);
	}
    }
    tq->icmds = newflags;
    return 1;
}


static void
deactivate_tqueue(tqueue * tq)
{
    tq->usage = NO_USAGE;

    tq->next = idle_tqueues;
    tq->prev = &idle_tqueues;
    if (idle_tqueues)
	idle_tqueues->prev = &(tq->next);
    idle_tqueues = tq;
}

static void
activate_tqueue(tqueue * tq)
{
    tqueue **qq = &active_tqueues;

    while (*qq && (*qq)->usage <= tq->usage)
	qq = &((*qq)->next);

    tq->next = *qq;
    tq->prev = 0;
    *qq = tq;
}

static void
ensure_usage(tqueue * tq)
{
    if (tq->usage == NO_USAGE) {
	tq->usage = active_tqueues ? active_tqueues->usage : 0;

	/* Remove tq from idle_tqueues... */
	*(tq->prev) = tq->next;
	if (tq->next)
	    tq->next->prev = tq->prev;

	/* ...and put it on active_tqueues. */
	activate_tqueue(tq);
    }
}

char *
default_flush_command(void)
{
    const char *str =
	server_string_option("default_flush_command", ".flush");

    return (str && str[0] != '\0') ? str_dup(str) : 0;
}

static tqueue *
find_tqueue(Objid player, int create_if_not_found)
{
    tqueue *tq;

    for (tq = active_tqueues; tq; tq = tq->next)
	if (tq->player == player)
	    return tq;

    for (tq = idle_tqueues; tq; tq = tq->next)
	if (tq->player == player)
	    return tq;

    if (!create_if_not_found)
	return 0;

    tq = (tqueue *) mymalloc(sizeof(tqueue), M_TASK);

    deactivate_tqueue(tq);

    tq->player = player;
    tq->handler = 0;
    tq->connected = 0;

    tq->first_input = tq->first_itail = tq->first_bg = 0;
    tq->last_input = &(tq->first_input);
    tq->last_itail = &(tq->first_itail);
    tq->last_bg = &(tq->first_bg);
    tq->total_input_length = tq->input_suspended = 0;

    tq->output_prefix = tq->output_suffix = 0;
    tq->flush_cmd = default_flush_command();
    tq->program_stream = 0;

    tq->reading = 0;
    tq->hold_input = 0;
    tq->disable_oob = 0;
    tq->icmds = ICMD_ALL_CMDS;
    tq->num_bg_tasks = 0;
    tq->last_input_task_id = 0;

    return tq;
}

static void
free_tqueue(tqueue * tq)
{
    /* Precondition: tq is on idle_tqueues */

    if (tq->output_prefix)
	free_str(tq->output_prefix);
    if (tq->output_suffix)
	free_str(tq->output_suffix);
    if (tq->flush_cmd)
	free_str(tq->flush_cmd);
    if (tq->program_stream)
	free_stream(tq->program_stream);
    if (tq->reading)
	free_vm(tq->reading_vm, 1);

    *(tq->prev) = tq->next;
    if (tq->next)
	tq->next->prev = tq->prev;

    myfree(tq, M_TASK);
}

static void
enqueue_bg_task(tqueue * tq, task * t)
{
    *(tq->last_bg) = t;
    tq->last_bg = &(t->next);
    t->next = 0;
}

static task *
dequeue_bg_task(tqueue * tq)
{
    task *t = tq->first_bg;

    if (t) {
	tq->first_bg = t->next;
	if (t->next == 0)
	    tq->last_bg = &(tq->first_bg);
	else
	    t->next = 0;
	tq->num_bg_tasks--;
    }
    return t;
}

static char oob_quote_prefix[] = OUT_OF_BAND_QUOTE_PREFIX;
#define oob_quote_prefix_length (sizeof(oob_quote_prefix) - 1)

enum dequeue_how { DQ_FIRST = -1, DQ_OOB = 0, DQ_INBAND = 1 };

static task *
dequeue_input_task(tqueue * tq, enum dequeue_how how)
{
    task *t;
    task **pt, **pitail;

    if (tq->disable_oob) {
	if (how == DQ_OOB)
	    return 0;
	how = DQ_FIRST;
    }

    if (!tq->first_input)
	return 0;
    else if (how == (tq->first_input->kind == TASK_OOB)) {
	pt = &(tq->first_itail->next);
	pitail = &(tq->first_itail->t.input.next_itail);
    } else {
	pt = &(tq->first_input);
	pitail = &(tq->first_itail);
    }
    t = *pt;

    if (t) {
	*pt = t->next;
	if (t->next == 0)
	    tq->last_input = pt;
	else
	    t->next = 0;

	if (t == *pitail) {
	    *pitail = 0;
	    if (t->t.input.next_itail) {
		tq->first_itail = t->t.input.next_itail;
		t->t.input.next_itail = 0;
	    }
	    if (*(tq->last_itail) == 0)
		tq->last_itail = &(tq->first_itail);
	}

	tq->total_input_length -= t->t.input.length;
	if (tq->input_suspended
	    && tq->connected && tq->total_input_length < INPUT_LOWAT) {
	    server_resume_input(tq->player);
	    tq->input_suspended = 0;
	}

	if (t->kind == TASK_OOB) {
	    if (tq->disable_oob)
		t->kind = TASK_INBAND;
	} else if (t->kind == TASK_QUOTED) {
	    if (!tq->disable_oob)
		memmove(t->t.input.string,
			t->t.input.string + oob_quote_prefix_length,
			1 + strlen(t->t.input.string +
				   oob_quote_prefix_length));
	    t->kind = TASK_INBAND;
	}
    }
    return t;
}

static void
free_task(task * t, int strong)
{				/* for FORKED tasks, strong == 1 means free the rt_env also.
				   for SUSPENDED tasks, strong == 1 means free the vm also. */
    switch (t->kind) {
    default:
	panic("Unknown task kind in free_task()");
	break;
    case TASK_BINARY:
    case TASK_INBAND:
    case TASK_QUOTED:
    case TASK_OOB:
	free_str(t->t.input.string);
	break;
    case TASK_FORKED:
	if (strong) {
	    free_rt_env(t->t.forked.rt_env,
			t->t.forked.program->num_var_names);
	    free_str(t->t.forked.a.verb);
	    free_str(t->t.forked.a.verbname);
	}
	free_program(t->t.forked.program);
	break;
    case TASK_SUSPENDED:
	if (strong)
	    free_vm(t->t.suspended.the_vm, 1);
	break;
    }
    myfree(t, M_TASK);
}

static int
new_task_id(void)
{
    int i;

    do {
	i = RANDOM();
    } while (i == 0);

    return i;
}

static void
start_programming(tqueue * tq, char *argstr)
{
    db_verb_handle h;
    const char *message, *vname;

    h = find_verb_for_programming(tq->player, argstr, &message, &vname);
    notify(tq->player, message);

    if (h.ptr) {
	tq->program_stream = new_stream(100);
	tq->program_object = db_verb_definer(h);
	tq->program_verb = str_dup(vname);
    }
}

struct state {
    Objid player;
    int nerrors;
    char *input;
};

static void
my_error(void *data, const char *msg)
{
    struct state *s = (state *) data;

    notify(s->player, msg);
    s->nerrors++;
}

static int
my_getc(void *data)
{
    struct state *s = (state *) data;

    if (*(s->input) != '\0')
	return *(s->input++);
    else
	return EOF;
}

static Parser_Client client = { my_error, 0, my_getc };

static void
end_programming(tqueue * tq)
{
    Objid player = tq->player;

    if (!valid(tq->program_object))
	notify(player, "That object appears to have disappeared ...");
    else {
	db_verb_handle h;
	Var desc;

	desc.type = (var_type) TYPE_STR;
	desc.v.str = tq->program_verb;
	h = find_described_verb(tq->program_object, desc);

	if (!h.ptr)
	    notify(player, "That verb appears to have disappeared ...");
	else {
	    struct state s;
	    Program *program;
	    char buf[30];

	    s.player = tq->player;
	    s.nerrors = 0;
	    s.input = stream_contents(tq->program_stream);

	    program = parse_program(current_version, client, &s);

	    sprintf(buf, "%d error(s).", s.nerrors);
	    notify(player, buf);

	    if (program) {
		db_set_verb_program(h, program);
		notify(player, "Verb programmed.");
	    } else
		notify(player, "Verb not programmed.");
	}
    }

    free_str(tq->program_verb);
    free_stream(tq->program_stream);
    tq->program_stream = 0;
}

static void
set_delimiter(char **slot, const char *string)
{
    if (*slot)
	free_str(*slot);
    if (*string == '\0')
	*slot = 0;
    else
	*slot = str_dup(string);
}

static int
find_verb_on(Objid oid, Parsed_Command * pc, db_verb_handle * vh)
{
    if (!valid(oid))
	return 0;

    *vh = db_find_command_verb(oid, pc->verb,
			       (pc->dobj == oid ? ASPEC_THIS
				: pc->dobj == NOTHING ? ASPEC_NONE
				: ASPEC_ANY),
			       pc->prep,
			       (pc->iobj == oid ? ASPEC_THIS
				: pc->iobj == NOTHING ? ASPEC_NONE
				: ASPEC_ANY));

    return vh->ptr != 0;
}


static int
do_intrinsic_command(tqueue * tq, Parsed_Command * pc)
{
    int icmd = icmd_index(pc->verb);
    if (!(icmd && (tq->icmds & (1 << icmd))))
	return 0;
    switch (icmd) {
    default:
	panic("Bad return value from icmd_index()");
	break;
    case ICMD_PROGRAM:
	if (!is_programmer(tq->player))
	    return 0;
	if (pc->args.v.list[0].v.num != 1)
	    notify(tq->player, "Usage:  .program object:verb");
	else
	    start_programming(tq, (char *) pc->args.v.list[1].v.str);
	break;
    case ICMD_PREFIX:
    case ICMD_OUTPUTPREFIX:
	set_delimiter(&(tq->output_prefix), pc->argstr);
	break;
    case ICMD_SUFFIX:
    case ICMD_OUTPUTSUFFIX:
	set_delimiter(&(tq->output_suffix), pc->argstr);
	break;
    }
    return 1;
}


static int
do_command_task(tqueue * tq, char *command)
{
    if (tq->program_stream) {	/* We're programming */
	if (strcmp(command, ".") == 0)	/* Done programming */
	    end_programming(tq);
	else
	    stream_printf(tq->program_stream, "%s\n", command);
    } else {
	Parsed_Command *pc = parse_command(command, tq->player);

	{
	    char *s = str_dup("");
	    run_server_task(tq->player, SYSTEM_OBJECT, "do_prompt",
			    parse_into_wordlist(s), s, 0);
	    free_str(s);
	}

	if (!pc)
	    return 0;

	if (!do_intrinsic_command(tq, pc)) {
	    Objid location = (valid(tq->player)
			      ? db_object_location(tq->player)
			      : NOTHING);
	    Objid self;
	    db_verb_handle vh;
	    Var result, args;

	    result.type = TYPE_INT;	/* for free_var() if task isn't DONE */
	    if (tq->output_prefix)
		notify(tq->player, tq->output_prefix);

	    args = parse_into_wordlist(command);
	    if (run_server_task_setting_id(tq->player, tq->handler,
					   "do_command", args, command,
					   &result,
					   &(tq->last_input_task_id))
		!= OUTCOME_DONE || is_true(result)) {
		/* Do nothing more; we assume :do_command handled it. */
	    } else if (find_verb_on(self = tq->player, pc, &vh)
		       || find_verb_on(self = location, pc, &vh)
		       || find_verb_on(self = pc->dobj, pc, &vh)
		       || find_verb_on(self = pc->iobj, pc, &vh)
		       || (valid(self = location)
			   && (vh = db_find_callable_verb(location, "huh"),
			       vh.ptr))) {
		do_input_task(tq->player, pc, self, vh);
	    } else {
		notify(tq->player, "I couldn't understand that.");
		tq->last_input_task_id = 0;
	    }

	    if (tq->output_suffix)
		notify(tq->player, tq->output_suffix);

	    free_var(result);

	    {
		char *s = str_dup("");
		run_server_task(tq->player, SYSTEM_OBJECT, "do_prompt",
				parse_into_wordlist(s), s, 0);
		free_str(s);
	    }
	}

	free_parsed_command(pc);
    }

    return 1;
}

static int
do_login_task(tqueue * tq, char *command)
{
    Var result;
    Var args;
    Objid old_max_object = db_last_used_objid();

    result.type = TYPE_INT;	/* In case #0:do_login_command does not exist
				 * or does not immediately return.
				 */

    args = parse_into_wordlist(command);
    run_server_task_setting_id(tq->player, tq->handler, "do_login_command",
			       args, command, &result,
			       &(tq->last_input_task_id));
    if (tq->connected && result.type == TYPE_OBJ && is_user(result.v.obj)) {
	Objid new_player = result.v.obj;
	Objid old_player = tq->player;
	tqueue *dead_tq = find_tqueue(new_player, 0);
	task *t;

	tq->player = new_player;
	if (tq->num_bg_tasks) {
	    /* Cute; this un-logged-in connection has some queued tasks!
	     * Must copy them over to their own tqueue for accounting...
	     */
	    tqueue *old_tq = find_tqueue(old_player, 1);

	    old_tq->num_bg_tasks = tq->num_bg_tasks;
	    while ((t = dequeue_bg_task(tq)) != 0)
		enqueue_bg_task(old_tq, t);
	    tq->num_bg_tasks = 0;
	}
	if (dead_tq) {		/* Copy over tasks from old queue for player */
	    tq->num_bg_tasks = dead_tq->num_bg_tasks;
	    while ((t = dequeue_input_task(dead_tq, DQ_FIRST)) != 0) {
		free_task(t, 0);
	    }
	    while ((t = dequeue_bg_task(dead_tq)) != 0) {
		enqueue_bg_task(tq, t);
	    }
	    dead_tq->player = NOTHING;	/* it'll be freed by run_ready_tasks */
	    dead_tq->num_bg_tasks = 0;
	}
	player_connected(old_player, new_player,
			 new_player > old_max_object);
    }
    free_var(result);
    return 1;
}

static void
do_out_of_band_command(tqueue * tq, char *command)
{
    run_server_task(tq->player, tq->handler, "do_out_of_band_command",
		    parse_into_wordlist(command), command, 0);
}

static int
is_out_of_input(tqueue * tq)
{
    return !tq->connected && !tq->first_input;
}

/*
 * Exported interface
 */

TaskQueue
new_task_queue(Objid player, Objid handler)
{
    TaskQueue result;
    result.ptr = (tqueue *) find_tqueue(player, 1);
    tqueue *tq = (tqueue *) result.ptr;

    tq->connected = 1;
    tq->handler = handler;

    return result;
}

void
free_task_queue(TaskQueue q)
{
    tqueue *tq = (tqueue *) q.ptr;

    tq->connected = 0;

    /* Activate this queue to ensure that the pending read()ing task will
     * eventually get resumed, even if there's no more input for it.
     */

    if (tq->reading)
	ensure_usage(tq);
}

#define TASK_CO_TABLE(DEFINE, tq, value, _)				\
    DEFINE(flush-command, _, TYPE_STR, str,				\
	   tq->flush_cmd ? str_ref(tq->flush_cmd) : str_dup(""),	\
	   {								\
	       if (tq->flush_cmd)					\
		   free_str(tq->flush_cmd);				\
	       if (value.type == TYPE_STR && value.v.str[0] != '\0')	\
		   tq->flush_cmd = str_ref(value.v.str);		\
	       else							\
		   tq->flush_cmd = 0;					\
	   })								\
									\
    DEFINE(hold-input, _, TYPE_INT, num,				\
	   tq->hold_input,						\
	   {								\
	       tq->hold_input = is_true(value);				\
	       /* Anything to be done? */				\
	       if (!tq->hold_input && tq->first_input)			\
		   ensure_usage(tq);					\
	   })								\
									\
    DEFINE(disable-oob, _, TYPE_INT, num,				\
	   tq->disable_oob,						\
	   {								\
	       tq->disable_oob = is_true(value);			\
	       /* Anything to be done? */				\
	       if (!tq->disable_oob && tq->first_input			\
		   && (tq->first_itail->next				\
		       || tq->first_input->kind == TASK_OOB))		\
		   ensure_usage(tq);					\
	   })								\
									\
    DEFINE(intrinsic-commands, _, TYPE_LIST, list,			\
           icmd_list(tq->icmds).v.list,					\
	   {								\
	       if (!icmd_set_flags(tq, value))				\
		   return 0;						\
	   })								\
				/*
				   int
				   tasks_set_connection_option(task_queue q, const char *option, Var value)
				   {
				   CONNECTION_OPTION_SET(TASK_CO_TABLE, (tqueue *)q.ptr, option, value);
				   }

				   int
				   tasks_connection_option(task_queue q, const char *option, Var * value)
				   {
				   CONNECTION_OPTION_GET(TASK_CO_TABLE, (tqueue *)q.ptr, option, value);
				   }

				   Var
				   tasks_connection_options(task_queue q, Var list)
				   {
				   CONNECTION_OPTION_LIST(TASK_CO_TABLE, (tqueue *)q.ptr, list);
				   }
				 */

int
tasks_set_connection_option(TaskQueue q, const char *option, Var value)
{
    do {
	if (!mystrcasecmp((option), "flush-command")) { {
		if (((tqueue *) q.ptr)->flush_cmd)
		    free_str(((tqueue *) q.ptr)->flush_cmd);
		if ((value).type == (_TYPE_STR | 0x80)
		    && (value).v.str[0] != '\0')
		    ((tqueue *) q.ptr)->flush_cmd = str_ref((value).v.str);
		else
		    ((tqueue *) q.ptr)->flush_cmd = 0;
	};
	return 1;
	}
	if (!mystrcasecmp((option), "hold-input")) { {
		((tqueue *) q.ptr)->hold_input = is_true((value));
		if (!((tqueue *) q.ptr)->hold_input
		    && ((tqueue *) q.ptr)->first_input)
		    ensure_usage(((tqueue *) q.ptr));
	};
	return 1;
	}
	if (!mystrcasecmp((option), "disable-oob")) { {
		((tqueue *) q.ptr)->disable_oob = is_true((value));
		if (!((tqueue *) q.ptr)->disable_oob
		    && ((tqueue *) q.ptr)->first_input
		    && (((tqueue *) q.ptr)->first_itail->next
			|| ((tqueue *) q.ptr)->first_input->kind ==
			TASK_OOB))
		    ensure_usage(((tqueue *) q.ptr));
	};
	return 1;
	}
	if (!mystrcasecmp((option), "intrinsic-commands")) { {
		if (!icmd_set_flags(((tqueue *) q.ptr), (value)))
		    return 0;
	};
	return 1;
	}
	return 0;
    } while (0);

}

int
tasks_connection_option(TaskQueue q, const char *option, Var * value)
{
    do {
	if (!mystrcasecmp((option), "flush-command")) {
	    (value)->type = (var_type) ((_TYPE_STR | 0x80));
	    (value)->v.str =
		(((tqueue *) q.ptr)->
		 flush_cmd ? str_ref(((tqueue *) q.ptr)->
				     flush_cmd) : str_dup(""));
	    return 1;
	}
	if (!mystrcasecmp((option), "hold-input")) {
	    (value)->type = (TYPE_INT);
	    (value)->v.num = (((tqueue *) q.ptr)->hold_input);
	    return 1;
	}
	if (!mystrcasecmp((option), "disable-oob")) {
	    (value)->type = (TYPE_INT);
	    (value)->v.num = (((tqueue *) q.ptr)->disable_oob);
	    return 1;
	}
	if (!mystrcasecmp((option), "intrinsic-commands")) {
	    (value)->type = (var_type) ((_TYPE_LIST | 0x80));
	    (value)->v.list =
		(icmd_list(((tqueue *) q.ptr)->icmds).v.list);
	    return 1;
	}
	return 0;
    } while (0);

}

Var
tasks_connection_options(TaskQueue q, Var list)
{
    do { {
	    Var pair = new_list(2);
	    pair.v.list[1].type = (var_type) (_TYPE_STR | 0x80);
	    pair.v.list[1].v.str = str_dup("flush-command");
	    pair.v.list[2].type = (var_type) ((_TYPE_STR | 0x80));
	    pair.v.list[2].v.str =
		(((tqueue *) q.ptr)->
		 flush_cmd ? str_ref(((tqueue *) q.ptr)->
				     flush_cmd) : str_dup(""));
	    (list) = listappend((list), pair);
    }
    {
	Var pair = new_list(2);
	pair.v.list[1].type = (var_type) (_TYPE_STR | 0x80);
	pair.v.list[1].v.str = str_dup("hold-input");
	pair.v.list[2].type = (TYPE_INT);
	pair.v.list[2].v.num = (((tqueue *) q.ptr)->hold_input);
	(list) = listappend((list), pair);
    }
    {
	Var pair = new_list(2);
	pair.v.list[1].type = (var_type) (_TYPE_STR | 0x80);
	pair.v.list[1].v.str = str_dup("disable-oob");
	pair.v.list[2].type = (TYPE_INT);
	pair.v.list[2].v.num = (((tqueue *) q.ptr)->disable_oob);
	(list) = listappend((list), pair);
    }
    {
	Var pair = new_list(2);
	pair.v.list[1].type = (var_type) (_TYPE_STR | 0x80);
	pair.v.list[1].v.str = str_dup("intrinsic-commands");
	pair.v.list[2].type = (var_type) ((_TYPE_LIST | 0x80));
	pair.v.list[2].v.list =
	    (icmd_list(((tqueue *) q.ptr)->icmds).v.list);
	(list) = listappend((list), pair);
    }
    return (list);
    } while (0);

}

#undef TASK_CO_TABLE

static void
enqueue_input_task(tqueue * tq, const char *input, int at_front,
		   int binary)
{
    static char oob_prefix[] = OUT_OF_BAND_PREFIX;
    task *t;

    t = (task *) mymalloc(sizeof(task), M_TASK);
    if (binary)
	t->kind = TASK_BINARY;
    else if (oob_quote_prefix_length > 0
	     && strncmp(oob_quote_prefix, input,
			oob_quote_prefix_length) == 0)
	t->kind = TASK_QUOTED;
    else if (sizeof(oob_prefix) > 1
	     && strncmp(oob_prefix, input, sizeof(oob_prefix) - 1) == 0)
	t->kind = TASK_OOB;
    else
	t->kind = TASK_INBAND;

    t->t.input.string = str_dup(input);
    tq->total_input_length += (t->t.input.length = strlen(input));

    t->t.input.next_itail = 0;
    if (at_front && tq->first_input) {	/* if nothing there, front == back */
	if ((tq->first_input->kind == TASK_OOB) != (t->kind == TASK_OOB)) {
	    t->t.input.next_itail = tq->first_itail;
	    tq->first_itail = t;
	    if (tq->last_itail == &(tq->first_itail))
		tq->last_itail = &(t->t.input.next_itail);
	}
	t->next = tq->first_input;
	tq->first_input = t;
    } else {
	if (tq->first_input && (((*(tq->last_itail))->kind == TASK_OOB)
				!= (t->kind == TASK_OOB)))
	    tq->last_itail = &((*(tq->last_itail))->t.input.next_itail);
	*(tq->last_itail) = t;

	*(tq->last_input) = t;
	tq->last_input = &(t->next);
	t->next = 0;
    }

    /* Anything to do with this line? */
    if (!tq->hold_input || tq->reading
	|| (!tq->disable_oob && t->kind == TASK_OOB))
	ensure_usage(tq);

    if (!tq->input_suspended
	&& tq->connected && tq->total_input_length > INPUT_HIWAT) {
	server_suspend_input(tq->player);
	tq->input_suspended = 1;
    }
}

void
task_suspend_input(TaskQueue q)
{
    tqueue *tq = (tqueue *) q.ptr;

    if (!tq->input_suspended && tq->connected) {
	server_suspend_input(tq->player);
	tq->input_suspended = 1;
    }
}

static void
flush_input(tqueue * tq, int show_messages)
{
    if (tq->first_input) {
	Stream *s = new_stream(100);
	task *t;

	if (show_messages)
	    notify(tq->player, ">> Flushing the following pending input:");
	while ((t = dequeue_input_task(tq, DQ_FIRST)) != 0) {
	    /* TODO*** flush only non-TASK_OOB tasks ??? */
	    if (show_messages) {
		stream_printf(s, ">>     %s", t->t.input.string);
		notify(tq->player, reset_stream(s));
	    }
	    free_task(t, 1);
	}
	if (show_messages)
	    notify(tq->player, ">> (Done flushing)");
    } else if (show_messages)
	notify(tq->player, ">> No pending input to flush...");
}

void
new_input_task(TaskQueue q, const char *input, int binary)
{
    tqueue *tq = (tqueue *) q.ptr;

    if (tq->flush_cmd && mystrcasecmp(input, tq->flush_cmd) == 0) {
	flush_input(tq, 1);
	return;
    }
    enqueue_input_task(tq, input, 0 /*at-rear */ , binary);
}

static void
enqueue_waiting(task * t)
{				/* either FORKED or SUSPENDED */

    struct timeval start_time = GET_START_TIME(t);
    Objid progr = (t->kind == TASK_FORKED
		   ? t->t.forked.a.progr
		   : progr_of_cur_verb(t->t.suspended.the_vm));
    tqueue *tq = find_tqueue(progr, 1);

    tq->num_bg_tasks++;
    if (!waiting_tasks
	|| timeval_lt(&start_time, &GET_START_TIME(waiting_tasks))) {
	t->next = waiting_tasks;
	waiting_tasks = t;
    } else {
	task *tt;

	for (tt = waiting_tasks; tt->next; tt = tt->next)
	    if (timeval_lt(&start_time, &GET_START_TIME(tt->next)))
		break;
	t->next = tt->next;
	tt->next = t;
    }
}

static void
enqueue_ft(Program * program, activation a, Var * rt_env,
	   int f_index, struct timeval start_time, int id)
{
    task *t = (task *) mymalloc(sizeof(task), M_TASK);

    t->kind = TASK_FORKED;
    t->t.forked.program = program;
    /* The next two lines were not present before 1.8.2.  a.rt_env/prog
     * were never accessed and were eventually overwritten by
     * forked.rt_env/program in do_forked_task().  Makes no sense to store
     * it two places, but here we are.
     * Setting it in the activation simplifies forked_task_bytes()
     */
    a.rt_env = rt_env;
    a.prog = program;
    a.base_rt_stack = NULL;
    a.top_rt_stack = NULL;
    t->t.forked.a = a;
    t->t.forked.rt_env = rt_env;
    t->t.forked.f_index = f_index;
    t->t.forked.start_time = start_time;
    t->t.forked.id = id;

    enqueue_waiting(t);
}

static int
check_user_task_limit(Objid user)
{
    tqueue *tq = find_tqueue(user, 0);
    int limit = -1;
    Var v;

    if (valid(user)
	&& db_find_property(user, "queued_task_limit", &v).ptr
	&& v.type == TYPE_INT)
	limit = v.v.num;

    if (limit < 0)
	limit = server_int_option("queued_task_limit", -1);

    if (limit < 0)
	return 1;
    else if ((tq ? tq->num_bg_tasks : 0) >= limit)
	return 0;
    else
	return 1;
}

enum error
enqueue_forked_task2(activation a, int f_index, unsigned after_seconds,
		     int vid)
{
    int id;
    Var *rt_env;
    struct timeval start_time;

    if (!check_user_task_limit(a.progr))
	return E_QUOTA;

    double_to_timeval((double) time(0) + after_seconds, &start_time);

    id = new_task_id();
    a.verb = str_ref(a.verb);
    a.verbname = str_ref(a.verbname);
    a.prog = program_ref(a.prog);
    if (vid >= 0) {
	free_var(a.rt_env[vid]);
	a.rt_env[vid].type = TYPE_INT;
	a.rt_env[vid].v.num = id;
    }
    rt_env = copy_rt_env(a.rt_env, a.prog->num_var_names);
    enqueue_ft(a.prog, a, rt_env, f_index, start_time, id);

    return E_NONE;
}

enum error
enqueue_suspended_task(vm the_vm, void *data)
{
    double after_seconds = *((double *) data);
    struct timeval now;
    struct timeval wait;
    task *t;

    gettimeofday(&now, NULL);
    double_to_timeval(after_seconds, &wait);

    if (check_user_task_limit(progr_of_cur_verb(the_vm))) {
	t = (task *) mymalloc(sizeof(task), M_TASK);
	t->kind = TASK_SUSPENDED;
	t->t.suspended.the_vm = the_vm;
	timeval_add(&t->t.suspended.start_time, &now, &wait);
	t->t.suspended.value = zero;

	enqueue_waiting(t);
	return E_NONE;
    } else
	return E_QUOTA;
}

void
resume_task(vm the_vm, Var value)
{
    task *t = (task *) mymalloc(sizeof(task), M_TASK);
    Objid progr = progr_of_cur_verb(the_vm);
    tqueue *tq = find_tqueue(progr, 1);

    t->kind = TASK_SUSPENDED;
    t->t.suspended.the_vm = the_vm;
    double_to_timeval(0.0, &t->t.suspended.start_time);	/* ready now */
    t->t.suspended.value = value;

    enqueue_bg_task(tq, t);
    ensure_usage(tq);
}

Var
read_input_now(Objid connection)
{
    tqueue *tq = find_tqueue(connection, 0);
    task *t;
    Var r;

    if (!tq || is_out_of_input(tq)) {
	r.type = TYPE_ERR;
	r.v.err = E_INVARG;
    } else if (!(t = dequeue_input_task(tq, DQ_INBAND))) {
	r.type = TYPE_INT;
	r.v.num = 0;
    } else {
	r.type = (var_type) TYPE_STR;
	r.v.str = t->t.input.string;
	myfree(t, M_TASK);
    }

    return r;
}

enum error
make_reading_task(vm the_vm, void *data)
{
    Objid player = *((Objid *) data);
    tqueue *tq = find_tqueue(player, 0);

    if (!tq || tq->reading || is_out_of_input(tq))
	return E_INVARG;
    else {
	tq->reading = 1;
	tq->reading_vm = the_vm;
	if (tq->first_input)	/* Anything to read? */
	    ensure_usage(tq);
	return E_NONE;
    }
}

int
last_input_task_id(Objid player)
{
    tqueue *tq = find_tqueue(player, 0);

    return tq ? tq->last_input_task_id : 0;
}

int
next_task_start(struct timeval *tv)
{
    tqueue *tq;

    for (tq = active_tqueues; tq; tq = tq->next)
	if (tq->first_input != 0 || tq->first_bg != 0) {
	    tv->tv_sec = 0;
	    tv->tv_usec = 0;
	    return 1;
	}

    if (waiting_tasks != 0) {
	struct timeval wait;
	struct timeval now;
	struct timeval zero;

	zero.tv_sec = 0;
	zero.tv_usec = 0;

	gettimeofday(&now, NULL);

	timeval_sub(&wait, &(waiting_tasks->kind == TASK_FORKED
			     ? waiting_tasks->t.forked.start_time
			     : waiting_tasks->t.suspended.start_time),
		    &now);
	*tv = timeval_lt(&wait, &zero) ? zero : wait;
	return 1;
    }

    return 0;
}

void
run_ready_tasks(void)
{
    task *t, *next_t;
    struct timeval now;
    tqueue *tq, *next_tq;

    gettimeofday(&now, NULL);

    for (t = waiting_tasks; t && timeval_lt(&GET_START_TIME(t), &now);
	 t = next_t) {
	Objid progr =
	    (t->kind ==
	     TASK_FORKED ? t->t.forked.a.progr : progr_of_cur_verb(t->t.
								   suspended.
								   the_vm));
	tqueue *tq = find_tqueue(progr, 1);

	next_t = t->next;
	ensure_usage(tq);
	enqueue_bg_task(tq, t);
    }
    waiting_tasks = t;

    {
	int did_one = 0;
	time_t start = time(0);

	while (active_tqueues && !did_one) {
	    /* Loop over tqueues, looking for a task */
	    tq = active_tqueues;

	    if (tq->reading && is_out_of_input(tq)) {
		Var v;

		tq->reading = 0;
		current_task_id = tq->reading_vm->task_id;
		v.type = TYPE_ERR;
		v.v.err = E_INVARG;
		resume_from_previous_vm(tq->reading_vm, v);
		did_one = 1;
	    }
	    while (!did_one) {	/* Loop over tasks, looking for runnable one */
		t = dequeue_input_task(tq,
				       ((tq->hold_input && !tq->reading)
					? DQ_OOB : DQ_FIRST));
		if (!t)
		    t = dequeue_bg_task(tq);
		if (!t)
		    break;

		switch (t->kind) {
		default:
		    panic("Unexpected task kind in run_ready_tasks()");
		    break;
		case TASK_OOB:
		    do_out_of_band_command(tq, t->t.input.string);
		    did_one = 1;
		    break;
		case TASK_BINARY:
		case TASK_INBAND:
		    if (tq->reading) {
			Var v;

			tq->reading = 0;
			current_task_id = tq->reading_vm->task_id;
			v.type = (var_type) TYPE_STR;
			v.v.str = t->t.input.string;
			resume_from_previous_vm(tq->reading_vm, v);
			did_one = 1;
		    } else {
			/* Used to insist on tq->connected here, but Pavel
			 * couldn't come up with a good reason to keep that
			 * restriction.
			 */
			add_command_to_history(tq->player,
					       t->t.input.string);
			did_one =
			    (tq->player >=
			     0 ? do_command_task : do_login_task) (tq,
								   t->t.
								   input.
								   string);
		    }
		    break;
		case TASK_FORKED:
		    {
			forked_task ft;

			ft = t->t.forked;
			current_task_id = ft.id;
			do_forked_task(ft.program, ft.rt_env, ft.a,
				       ft.f_index);
			did_one = 1;
		    }
		    break;
		case TASK_SUSPENDED:
		    current_task_id = t->t.suspended.the_vm->task_id;
		    resume_from_previous_vm(t->t.suspended.the_vm,
					    t->t.suspended.value);
		    did_one = 1;
		    break;
		}
		free_task(t, 0);
	    }

	    active_tqueues = tq->next;

	    if (did_one) {
		/* Bump the usage level of this tqueue */
		time_t end = time(0);

		tq->usage += end - start;
		activate_tqueue(tq);
	    } else {
		/* There was nothing to do on this tqueue, so deactivate it */
		deactivate_tqueue(tq);
	    }
	}
    }

    /* Free any unconnected and empty tqueues */
    for (tq = idle_tqueues; tq; tq = next_tq) {
	next_tq = tq->next;

	if (!tq->connected && !tq->first_input && tq->num_bg_tasks == 0)
	    free_tqueue(tq);
    }
}

enum outcome
run_server_task(Objid player, Objid what, const char *verb, Var args,
		const char *argstr, Var * result)
{
    return run_server_task_setting_id(player, what, verb, args, argstr,
				      result, 0);
}

enum outcome
run_server_task_setting_id(Objid player, Objid what, const char *verb,
			   Var args, const char *argstr, Var * result,
			   int *task_id)
{
    db_verb_handle h;

    current_task_id = new_task_id();
    if (task_id)
	*task_id = current_task_id;
    h = db_find_callable_verb(what, verb);
    if (h.ptr)
	return do_server_verb_task(what, verb, args, h, player, argstr,
				   result, 1 /*traceback */ );
    else {
	/* simulate an empty verb */
	if (result) {
	    result->type = TYPE_INT;
	    result->v.num = 0;
	}
	free_var(args);
	return OUTCOME_DONE;
    }
}

enum outcome
run_server_program_task(Objid self, const char *verb, Var args, Objid vloc,
			const char *verbname, Program * program,
			Objid progr, int debug, Objid player,
			const char *argstr, Var * result)
{
    current_task_id = new_task_id();
    return do_server_program_task(self, verb, args, vloc, verbname,
				  program, progr, debug, player, argstr,
				  result, 1 /*traceback */ );
}

void
register_task_queue(task_enumerator enumerator)
{
    ext_queue *eq = (ext_queue *) mymalloc(sizeof(ext_queue), M_TASK);

    eq->enumerator = enumerator;
    eq->next = external_queues;
    external_queues = eq;
}

static void
write_forked_task(forked_task ft)
{
    int lineno = find_line_number(ft.program, ft.f_index, 0);

    dbio_printf("0 %d %lf %d\n", lineno, timeval_to_double(&ft.start_time),
		ft.id);
    write_activ_as_pi(ft.a);
    write_rt_env(ft.program->var_names, ft.rt_env,
		 ft.program->num_var_names);
    dbio_write_forked_program(ft.program, ft.f_index);
}

static void
write_suspended_task(suspended_task st)
{
    dbio_printf("%lf %d ", timeval_to_double(&st.start_time),
		st.the_vm->task_id);
    dbio_write_var(st.value);
    write_vm(st.the_vm);
}

void
write_task_queue(void)
{
    int forked_count = 0;
    int suspended_count = 0;
    task *t;
    tqueue *tq;

    dbio_printf("0 clocks\n");	/* for compatibility's sake */

    for (t = waiting_tasks; t; t = t->next)
	if (t->kind == TASK_FORKED)
	    forked_count++;
	else			/* t->kind == TASK_SUSPENDED */
	    suspended_count++;

    for (tq = active_tqueues; tq; tq = tq->next)
	for (t = tq->first_bg; t; t = t->next)
	    if (t->kind == TASK_FORKED)
		forked_count++;
	    else		/* t->kind == TASK_SUSPENDED */
		suspended_count++;


    dbio_printf("%d queued tasks\n", forked_count);

    for (t = waiting_tasks; t; t = t->next)
	if (t->kind == TASK_FORKED)
	    write_forked_task(t->t.forked);

    for (tq = active_tqueues; tq; tq = tq->next)
	for (t = tq->first_bg; t; t = t->next)
	    if (t->kind == TASK_FORKED)
		write_forked_task(t->t.forked);

    dbio_printf("%d suspended tasks\n", suspended_count);

    for (t = waiting_tasks; t; t = t->next)
	if (t->kind == TASK_SUSPENDED)
	    write_suspended_task(t->t.suspended);

    for (tq = active_tqueues; tq; tq = tq->next)
	for (t = tq->first_bg; t; t = t->next)
	    if (t->kind == TASK_SUSPENDED)
		write_suspended_task(t->t.suspended);
}

int
read_task_queue(void)
{
    int count, dummy, suspended_count, suspended_task_header;

    /* Skip obsolete clock stuff */
    if (dbio_scanf("%d clocks\n", &count) != 1) {
	errlog("READ_TASK_QUEUE: Bad clock count.\n");
	return 0;
    }
    for (; count > 0; count--)
	/* I use a `dummy' variable here and elsewhere instead of the `*'
	 * assignment-suppression syntax of `scanf' because it allows more
	 * straightforward error checking; unfortunately, the standard says
	 * that suppressed assignments are not counted in determining the
	 * returned value of `scanf'...
	 */
	if (dbio_scanf("%d %d %d\n", &dummy, &dummy, &dummy) != 3) {
	    errlog("READ_TASK_QUEUE: Bad clock; count = %d\n", count);
	    return 0;
	}
    if (dbio_scanf("%d queued tasks\n", &count) != 1) {
	errlog("READ_TASK_QUEUE: Bad task count.\n");
	return 0;
    }
    for (; count > 0; count--) {
	int first_lineno, id, old_size;
	double st;
	char c;
	struct timeval start_time;
	Program *program;
	Var *rt_env, *old_rt_env;
	const char **old_names;
	activation a;

	if (dbio_scanf("%d %d %lf %d%c",
		       &dummy, &first_lineno, &st, &id, &c) != 5
	    || c != '\n') {
	    errlog("READ_TASK_QUEUE: Bad numbers, count = %d.\n", count);
	    return 0;
	}
	double_to_timeval(st, &start_time);

	if (!read_activ_as_pi(&a)) {
	    errlog("READ_TASK_QUEUE: Bad activation, count = %d.\n",
		   count);
	    return 0;
	}
	if (!read_rt_env(&old_names, &old_rt_env, &old_size)) {
	    errlog("READ_TASK_QUEUE: Bad env, count = %d.\n", count);
	    return 0;
	}
	if (!(program = dbio_read_program(dbio_input_version,
					  0, (void *) "forked task"))) {
	    errlog("READ_TASK_QUEUE: Bad program, count = %d.\n", count);
	    return 0;
	}
	rt_env = reorder_rt_env(old_rt_env, old_names, old_size, program);
	program->first_lineno = first_lineno;

	enqueue_ft(program, a, rt_env, MAIN_VECTOR, start_time, id);
    }

    suspended_task_header = dbio_scanf("%d suspended tasks\n",
				       &suspended_count);
    if (suspended_task_header == EOF)
	return 1;		/* old version */

    if (suspended_task_header != 1) {
	errlog("READ_TASK_QUEUE: Bad suspended task count.\n");
	return 0;
    }
    for (; suspended_count > 0; suspended_count--) {
	task *t = (task *) mymalloc(sizeof(task), M_TASK);
	int task_id;
	double st;
	char c;

	t->kind = TASK_SUSPENDED;
	if (dbio_scanf("%lf %d%c", &st, &task_id, &c) != 3) {
	    errlog
		("READ_TASK_QUEUE: Bad suspended task header, count = %d\n",
		 suspended_count);
	    return 0;
	}
	double_to_timeval(st, &t->t.suspended.start_time);
	if (c == ' ')
	    t->t.suspended.value = dbio_read_var();
	else if (c == '\n')
	    t->t.suspended.value = zero;
	else {
	    errlog
		("READ_TASK_QUEUE: Bad suspended task value, count = %d\n",
		 suspended_count);
	    return 0;
	}

	if (!(t->t.suspended.the_vm = read_vm(task_id))) {
	    errlog("READ_TASK_QUEUE: Bad suspended task vm, count = %d\n",
		   suspended_count);
	    return 0;
	}
	enqueue_waiting(t);
    }
    return 1;
}

db_verb_handle
find_verb_for_programming(Objid player, const char *verbref,
			  const char **message, const char **vname)
{
    char *copy = str_dup(verbref);
    char *colon = strchr(copy, ':');
    char *obj;
    Objid oid;
    db_verb_handle h;
    static Stream *str = 0;
    Var desc;

    if (!str)
	str = new_stream(100);

    h.ptr = 0;

    if (!colon || colon[1] == '\0') {
	free_str(copy);
	*message = "You must specify a verb; use the format object:verb.";
	return h;
    }
    *colon = '\0';
    obj = copy;
    *vname = verbref + (colon - copy) + 1;

    if (obj[0] == '$')
	oid = get_system_object(obj + 1);
    else
	oid = match_object(player, obj);

    if (!valid(oid)) {
	switch (oid) {
	case FAILED_MATCH:
	    stream_printf(str, "I don't see \"%s\" here.", obj);
	    break;
	case AMBIGUOUS:
	    stream_printf(str, "I don't know which \"%s\" you mean.", obj);
	    break;
	default:
	    stream_printf(str, "\"%s\" is not a valid object.", obj);
	    break;
	}
	*message = reset_stream(str);
	free_str(copy);
	return h;
    }
    desc.type = (var_type) TYPE_STR;
    desc.v.str = *vname;
    h = find_described_verb(oid, desc);
    free_str(copy);

    if (!h.ptr)
	*message = "That object does not have that verb definition.";
    else if (!db_verb_allows(h, player, VF_WRITE)
	     || (server_flag_option("protect_set_verb_code")
		 && !is_wizard(player))) {
	*message = "Permission denied.";
	h.ptr = 0;
    } else {
	stream_printf(str, "Now programming %s:%s.  Use \".\" to end.",
		      db_object_name(oid), db_verb_names(h));
	*message = reset_stream(str);
    }

    return h;
}

static package
bf_queue_info(Var arglist, Byte next, void *vdata, Objid progr)
{
    int nargs = arglist.v.list[0].v.num;
    Var res;

    if (nargs == 0) {
	int count = 0;
	tqueue *tq;

	for (tq = active_tqueues; tq; tq = tq->next)
	    count++;
	for (tq = idle_tqueues; tq; tq = tq->next)
	    count++;

	res = new_list(count);
	for (tq = active_tqueues; tq; tq = tq->next) {
	    res.v.list[count].type = TYPE_OBJ;
	    res.v.list[count].v.obj = tq->player;
	    count--;
	}
	for (tq = idle_tqueues; tq; tq = tq->next) {
	    res.v.list[count].type = TYPE_OBJ;
	    res.v.list[count].v.obj = tq->player;
	    count--;
	}
    } else {
	Objid who = arglist.v.list[1].v.obj;
	tqueue *tq = find_tqueue(who, 0);

	res.type = TYPE_INT;
	res.v.num = (tq ? tq->num_bg_tasks : 0);
    }

    free_var(arglist);
    return make_var_pack(res);
}

static package
bf_task_id(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var r;
    r.type = TYPE_INT;
    r.v.num = current_task_id;
    free_var(arglist);
    return make_var_pack(r);
}

static int
activation_bytes(activation * ap)
{
    int total = sizeof(activation);
    Var *v;
    int i;

    /* The MOO Way [tm] is double-billing to avoid the possibility
     * of not billing at all, so the size of the prog is counted here
     * even though it will be shared unless the verb was reprogrammed.
     */
    total += program_bytes(ap->prog);
    for (i = 0; i < ap->prog->num_var_names; ++i)
	total += value_bytes(ap->rt_env[i]);
    if (ap->top_rt_stack) {
	for (v = ap->top_rt_stack - 1; v >= ap->base_rt_stack; v--)
	    total += value_bytes(*v);
    }
    /* XXX ignore bi_func_data, it's an opaque type. */
    total += value_bytes(ap->temp) - sizeof(Var);
    total += memo_strlen(ap->verb) + 1;
    total += memo_strlen(ap->verbname) + 1;
    return total;
}

static int
forked_task_bytes(forked_task ft)
{
    int total = sizeof(forked_task);

    /* ft.program is duplicated in ft.a */
    total += activation_bytes(&ft.a) - sizeof(activation);
    /* ft.rt_env is properly inside ft.a now */

    return total;
}

static Var
list_for_forked_task(forked_task ft)
{
    Var list;

    list = new_list(10);
    list.v.list[1].type = TYPE_INT;
    list.v.list[1].v.num = ft.id;
    list.v.list[2].type = TYPE_INT;
    list.v.list[2].v.num = ft.start_time.tv_sec;
    list.v.list[3] = new_float(timeval_to_double(&ft.start_time));	/* was clock ID, now float start time */
    list.v.list[4].type = TYPE_INT;
    list.v.list[4].v.num = DEFAULT_BG_TICKS;	/* OBSOLETE: was clock ticks */
    list.v.list[5].type = TYPE_OBJ;
    list.v.list[5].v.obj = ft.a.progr;
    list.v.list[6].type = TYPE_OBJ;
    list.v.list[6].v.obj = ft.a.vloc;
    list.v.list[7].type = (var_type) TYPE_STR;
    list.v.list[7].v.str = str_ref(ft.a.verbname);
    list.v.list[8].type = TYPE_INT;
    list.v.list[8].v.num = find_line_number(ft.program, ft.f_index, 0);
    list.v.list[9].type = TYPE_OBJ;
    list.v.list[9].v.obj = ft.a.self;
    list.v.list[10].type = TYPE_INT;
    list.v.list[10].v.num = forked_task_bytes(ft);

    return list;
}

static int
suspended_task_bytes(vm the_vm)
{
    int total = sizeof(vmstruct);
    int i;

    for (i = 0; i <= the_vm->top_activ_stack; i++)
	total += activation_bytes(the_vm->activ_stack + i);

    return total;
}

static Var
list_for_vm(vm the_vm)
{
    Var list;

    list = new_list(10);

    list.v.list[1].type = TYPE_INT;
    list.v.list[1].v.num = the_vm->task_id;

    list.v.list[3].type = TYPE_INT;
    list.v.list[3].v.num = 0;	/* was clock ID, now float start time (but not set here) */
    list.v.list[4].type = TYPE_INT;
    list.v.list[4].v.num = DEFAULT_BG_TICKS;	/* OBSOLETE: was clock ticks */
    list.v.list[5].type = TYPE_OBJ;
    list.v.list[5].v.obj = progr_of_cur_verb(the_vm);
    list.v.list[6].type = TYPE_OBJ;
    list.v.list[6].v.obj = top_activ(the_vm).vloc;
    list.v.list[7].type = (var_type) TYPE_STR;
    list.v.list[7].v.str = str_ref(top_activ(the_vm).verbname);
    list.v.list[8].type = TYPE_INT;
    list.v.list[8].v.num = suspended_lineno_of_vm(the_vm);
    list.v.list[9].type = TYPE_OBJ;
    list.v.list[9].v.obj = top_activ(the_vm).self;
    list.v.list[10].type = TYPE_INT;
    list.v.list[10].v.num = suspended_task_bytes(the_vm);

    return list;
}

static Var
list_for_suspended_task(suspended_task st)
{
    Var list;

    list = list_for_vm(st.the_vm);
    list.v.list[2].type = TYPE_INT;
    list.v.list[2].v.num = st.start_time.tv_sec;
    list.v.list[3] = new_float(timeval_to_double(&st.start_time));

    return list;
}

static Var
list_for_reading_task(Objid player, vm the_vm)
{
    Var list;

    list = list_for_vm(the_vm);
    list.v.list[2].type = TYPE_INT;
    list.v.list[2].v.num = -1;	/* conventional value */
    list.v.list[3] = new_float(0.0);

    list.v.list[5].v.obj = player;

    return list;
}

struct qcl_data {
    Objid progr;
    int show_all;
    int i;
    Var tasks;
};

static task_enum_action
counting_closure(vm the_vm, const char *status, void *data)
{
    struct qcl_data *qdata = (qcl_data *) data;

    if (qdata->show_all || qdata->progr == progr_of_cur_verb(the_vm))
	qdata->i++;

    return TEA_CONTINUE;
}

static task_enum_action
listing_closure(vm the_vm, const char *status, void *data)
{
    struct qcl_data *qdata = (qcl_data *) data;
    Var list;

    if (qdata->show_all || qdata->progr == progr_of_cur_verb(the_vm)) {
	list = list_for_vm(the_vm);
	list.v.list[2].type = (var_type) TYPE_STR;
	list.v.list[2].v.str = str_dup(status);
	qdata->tasks.v.list[qdata->i++] = list;
    }
    return TEA_CONTINUE;
}

static package
bf_queued_tasks(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var tasks;
    int show_all = is_wizard(progr);
    tqueue *tq;
    task *t;
    int i, count = 0;
    ext_queue *eq;
    struct qcl_data qdata;

    for (tq = idle_tqueues; tq; tq = tq->next) {
	if (tq->reading && (show_all || tq->player == progr))
	    count++;
    }

    for (tq = active_tqueues; tq; tq = tq->next) {
	if (tq->reading && (show_all || tq->player == progr))
	    count++;

	for (t = tq->first_bg; t; t = t->next)
	    if (show_all
		|| (t->kind == TASK_FORKED
		    ? t->t.forked.a.progr == progr
		    : progr_of_cur_verb(t->t.suspended.the_vm) == progr))
		count++;
    }

    for (t = waiting_tasks; t; t = t->next)
	if (show_all
	    || (t->kind == TASK_FORKED
		? t->t.forked.a.progr == progr
		: progr_of_cur_verb(t->t.suspended.the_vm) == progr))
	    count++;

    qdata.progr = progr;
    qdata.show_all = show_all;
    qdata.i = count;
    for (eq = external_queues; eq; eq = eq->next)
	(*eq->enumerator) (counting_closure, &qdata);
    count = qdata.i;

    tasks = new_list(count);
    i = 1;

    for (tq = idle_tqueues; tq; tq = tq->next) {
	if (tq->reading && (show_all || tq->player == progr))
	    tasks.v.list[i++] = list_for_reading_task(tq->player,
						      tq->reading_vm);
    }

    for (tq = active_tqueues; tq; tq = tq->next) {
	if (tq->reading && (show_all || tq->player == progr))
	    tasks.v.list[i++] = list_for_reading_task(tq->player,
						      tq->reading_vm);

	for (t = tq->first_bg; t; t = t->next)
	    if (t->kind == TASK_FORKED && (show_all
					   || t->t.forked.a.progr ==
					   progr))
		tasks.v.list[i++] = list_for_forked_task(t->t.forked);
	    else if (t->kind == TASK_SUSPENDED
		     && (show_all
			 || progr_of_cur_verb(t->t.suspended.the_vm) ==
			 progr))
		tasks.v.list[i++] =
		    list_for_suspended_task(t->t.suspended);
    }

    for (t = waiting_tasks; t; t = t->next) {
	if (t->kind == TASK_FORKED && (show_all ||
				       t->t.forked.a.progr == progr))
	    tasks.v.list[i++] = list_for_forked_task(t->t.forked);
	else if (t->kind == TASK_SUSPENDED
		 && (progr_of_cur_verb(t->t.suspended.the_vm) == progr
		     || show_all))
	    tasks.v.list[i++] = list_for_suspended_task(t->t.suspended);
    }

    qdata.tasks = tasks;
    qdata.i = i;
    for (eq = external_queues; eq; eq = eq->next)
	(*eq->enumerator) (listing_closure, &qdata);

    free_var(arglist);
    return make_var_pack(tasks);
}

struct fcl_data {
    int id;
    vm the_vm;
};

static task_enum_action
finding_closure(vm the_vm, const char *status, void *data)
{
    struct fcl_data *fdata = (fcl_data *) data;

    if (the_vm->task_id == fdata->id) {
	fdata->the_vm = the_vm;
	return TEA_STOP;
    }
    return TEA_CONTINUE;
}

vm
find_suspended_task(int id)
{
    tqueue *tq;
    task *t;
    ext_queue *eq;
    struct fcl_data fdata;

    for (t = waiting_tasks; t; t = t->next)
	if (t->kind == TASK_SUSPENDED
	    && t->t.suspended.the_vm->task_id == id)
	    return t->t.suspended.the_vm;

    for (tq = idle_tqueues; tq; tq = tq->next)
	if (tq->reading && tq->reading_vm->task_id == id)
	    return tq->reading_vm;

    for (tq = active_tqueues; tq; tq = tq->next) {
	if (tq->reading && tq->reading_vm->task_id == id)
	    return tq->reading_vm;

	for (t = tq->first_bg; t; t = t->next)
	    if (t->kind == TASK_SUSPENDED
		&& t->t.suspended.the_vm->task_id == id)
		return t->t.suspended.the_vm;
    }

    fdata.id = id;

    for (eq = external_queues; eq; eq = eq->next)
	switch ((*eq->enumerator) (finding_closure, &fdata)) {
	case TEA_CONTINUE:
	    /* Do nothing; continue searching other queues */
	    break;
	case TEA_KILL:
	    panic("Can't happen in FIND_SUSPENDED_TASK!");
	case TEA_STOP:
	    return fdata.the_vm;
	}

    return 0;
}

struct kcl_data {
    int id;
    Objid owner;
};

static task_enum_action
killing_closure(vm the_vm, const char *status, void *data)
{
    struct kcl_data *kdata = (kcl_data *) data;

    if (the_vm->task_id == kdata->id) {
	if (is_wizard(kdata->owner)
	    || progr_of_cur_verb(the_vm) == kdata->owner) {
	    free_vm(the_vm, 1);
	    return TEA_KILL;
	} else
	    return TEA_STOP;
    }
    return TEA_CONTINUE;
}

static enum error
kill_task(int id, Objid owner)
{
    task **tt;
    tqueue *tq;

    if (id == current_task_id) {
	return E_NONE;
    }
    for (tt = &waiting_tasks; *tt; tt = &((*tt)->next)) {
	task *t = *tt;
	Objid progr;

	if (t->kind == TASK_FORKED && t->t.forked.id == id)
	    progr = t->t.forked.a.progr;
	else if (t->kind == TASK_SUSPENDED
		 && t->t.suspended.the_vm->task_id == id)
	    progr = progr_of_cur_verb(t->t.suspended.the_vm);
	else
	    continue;

	if (!is_wizard(owner) && owner != progr)
	    return E_PERM;
	tq = find_tqueue(progr, 0);
	if (tq)
	    tq->num_bg_tasks--;
	*tt = t->next;
	free_task(t, 1);
	return E_NONE;
    }

    for (tq = idle_tqueues; tq; tq = tq->next) {
	if (tq->reading && tq->reading_vm->task_id == id) {
	    if (!is_wizard(owner) && owner != tq->player)
		return E_PERM;
	    free_vm(tq->reading_vm, 1);
	    tq->reading = 0;
	    return E_NONE;
	}
    }

    for (tq = active_tqueues; tq; tq = tq->next) {

	if (tq->reading && tq->reading_vm->task_id == id) {
	    if (!is_wizard(owner) && owner != tq->player)
		return E_PERM;
	    free_vm(tq->reading_vm, 1);
	    tq->reading = 0;
	    return E_NONE;
	}
	for (tt = &(tq->first_bg); *tt; tt = &((*tt)->next)) {
	    task *t = *tt;

	    if ((t->kind == TASK_FORKED && t->t.forked.id == id)
		|| (t->kind == TASK_SUSPENDED
		    && t->t.suspended.the_vm->task_id == id)) {
		if (!is_wizard(owner) && owner != tq->player)
		    return E_PERM;
		*tt = t->next;
		if (t->next == 0)
		    tq->last_bg = tt;
		tq->num_bg_tasks--;
		free_task(t, 1);
		return E_NONE;
	    }
	}
    }

    {
	struct kcl_data kdata;
	ext_queue *eq;

	kdata.id = id;
	kdata.owner = owner;
	for (eq = external_queues; eq; eq = eq->next)
	    switch ((*eq->enumerator) (killing_closure, &kdata)) {
	    case TEA_CONTINUE:
		/* Do nothing; continue searching other queues */
		break;
	    case TEA_KILL:
		return E_NONE;
	    case TEA_STOP:
		return E_PERM;
	    }
    }

    return E_INVARG;
}

static package
bf_kill_task(Var arglist, Byte next, void *vdata, Objid progr)
{
    int id = arglist.v.list[1].v.num;
    enum error e = kill_task(id, progr);

    free_var(arglist);
    if (e != E_NONE)
	return make_error_pack(e);
    else if (id == current_task_id)
	return make_kill_pack();

    return no_var_pack();
}

static enum error
do_resume(int id, Var value, Objid progr)
{
    task **tt;
    tqueue *tq;

    for (tt = &waiting_tasks; *tt; tt = &((*tt)->next)) {
	task *t = *tt;
	Objid owner;

	if (t->kind == TASK_SUSPENDED
	    && t->t.suspended.the_vm->task_id == id)
	    owner = progr_of_cur_verb(t->t.suspended.the_vm);
	else
	    continue;

	if (!is_wizard(progr) && progr != owner)
	    return E_PERM;
	double_to_timeval(0.0, &t->t.suspended.start_time);	/* runnable now */
	free_var(t->t.suspended.value);
	t->t.suspended.value = value;
	tq = find_tqueue(owner, 1);
	*tt = t->next;
	ensure_usage(tq);
	enqueue_bg_task(tq, t);
	return E_NONE;
    }

    for (tq = active_tqueues; tq; tq = tq->next) {
	for (tt = &(tq->first_bg); *tt; tt = &((*tt)->next)) {
	    task *t = *tt;

	    if (t->kind == TASK_SUSPENDED
		&& t->t.suspended.the_vm->task_id == id) {
		if (!is_wizard(progr) && progr != tq->player)
		    return E_PERM;
		/* already resumed, but we have a new value for it */
		free_var(t->t.suspended.value);
		t->t.suspended.value = value;
		return E_NONE;
	    }
	}
    }

    return E_INVARG;
}

static package
bf_resume(Var arglist, Byte next, void *vdata, Objid progr)
{
    int nargs = arglist.v.list[0].v.num;
    Var value;
    enum error e;

    value = (nargs >= 2 ? var_ref(arglist.v.list[2]) : zero);
    e = do_resume(arglist.v.list[1].v.num, value, progr);
    free_var(arglist);
    if (e != E_NONE) {
	free_var(value);
	return make_error_pack(e);
    }
    return no_var_pack();
}

static package
bf_output_delimiters(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var r;
    Objid player = arglist.v.list[1].v.obj;

    free_var(arglist);

    if (!is_wizard(progr) && progr != player)
	return make_error_pack(E_PERM);
    else {
	const char *prefix, *suffix;
	tqueue *tq = find_tqueue(player, 0);

	if (!tq || !tq->connected)
	    return make_error_pack(E_INVARG);

	if (tq->output_prefix)
	    prefix = tq->output_prefix;
	else
	    prefix = "";

	if (tq->output_suffix)
	    suffix = tq->output_suffix;
	else
	    suffix = "";

	r = new_list(2);
	r.v.list[1].type = (var_type) TYPE_STR;
	r.v.list[2].type = (var_type) TYPE_STR;
	r.v.list[1].v.str = str_dup(prefix);
	r.v.list[2].v.str = str_dup(suffix);
    }
    return make_var_pack(r);
}

static package
bf_force_input(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (conn, string [, at_front]) */
    Objid conn = arglist.v.list[1].v.obj;
    const char *line = arglist.v.list[2].v.str;
    int at_front = (arglist.v.list[0].v.num > 2
		    && is_true(arglist.v.list[3]));
    tqueue *tq;

    if (!is_wizard(progr) && progr != conn) {
	free_var(arglist);
	return make_error_pack(E_PERM);
    }
    tq = find_tqueue(conn, 1);
    enqueue_input_task(tq, line, at_front, 0 /*non-binary */ );
    free_var(arglist);
    return no_var_pack();
}

static package
bf_flush_input(Var arglist, Byte next, void *vdata, Objid progr)
{				/* (conn [, show_messages]) */
    Objid conn = arglist.v.list[1].v.obj;
    int show_messages = (arglist.v.list[0].v.num > 1
			 && is_true(arglist.v.list[2]));
    tqueue *tq;

    if (!is_wizard(progr) && progr != conn) {
	free_var(arglist);
	return make_error_pack(E_PERM);
    }
    tq = find_tqueue(conn, 1);
    flush_input(tq, show_messages);
    free_var(arglist);
    return no_var_pack();
}

void
register_tasks(void)
{
    register_function("task_id", 0, 0, bf_task_id);
    register_function("queued_tasks", 0, 0, bf_queued_tasks);
    register_function("kill_task", 1, 1, bf_kill_task, TYPE_INT);
    register_function("output_delimiters", 1, 1, bf_output_delimiters,
		      TYPE_OBJ);
    register_function("queue_info", 0, 1, bf_queue_info, TYPE_OBJ);
    register_function("resume", 1, 2, bf_resume, TYPE_INT, TYPE_ANY);
    register_function("force_input", 2, 3, bf_force_input,
		      TYPE_OBJ, TYPE_STR, TYPE_ANY);
    register_function("flush_input", 1, 2, bf_flush_input, TYPE_OBJ,
		      TYPE_ANY);
}

