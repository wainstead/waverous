/******************************************************************************
  Copyright (c) 1995, 1996 Xerox Corporation.  All rights reserved.
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

/* Extensions to the MOO server

 * This module contains some examples of how to extend the MOO server using
 * some of the interfaces exported by various other modules.  The examples are
 * all commented out, since they're really not all that useful in general; they
 * were written primarily to test out the interfaces they use.
 *
 * The uncommented parts of this module provide a skeleton for any module that
 * implements new MOO built-in functions.  Feel free to replace the
 * commented-out bits with your own extensions; in future releases, you can
 * just replace the distributed version of this file (which will never contain
 * any actually useful code) with your own edited version as a simple way to
 * link in your extensions.
 */

#define EXAMPLE 0

#include "config.h"
#include "bf_register.h"
#include "functions.h"
#include "db_tune.h"

#include "my-unistd.h"

#include "exceptions.h"
#include "log.h"
#include "net_multi.h"
#include "storage.h"
#include "tasks.h"

/* FUP extension */
#include "structures.h"
#include "utils.h"
/* FUP extension */

/* vrandom extension */
#include "list.h"
/* vrandom extension */

#include <math.h>
#include <stdlib.h>


/* utime - andy */
#include <sys/time.h>		/* these two for gettimeofday() */
#include <unistd.h>
#include "numbers.h"		/* for new_float() */
#include "exceptions.h"		/* for panic() */
/* utime - andy */

#if EXAMPLE

typedef struct stdin_waiter {
    struct stdin_waiter *next;
    vm the_vm;
} stdin_waiter;

static stdin_waiter *waiters = 0;

static task_enum_action
stdin_enumerator(task_closure closure, void *data)
{
    stdin_waiter **ww;

    for (ww = &waiters; *ww; ww = &((*ww)->next)) {
	stdin_waiter *w = *ww;
	const char *status = (w->the_vm->task_id & 1
			      ? "stdin-waiting" : "stdin-weighting");
	task_enum_action tea = (*closure) (w->the_vm, status, data);

	if (tea == TEA_KILL) {
	    *ww = w->next;
	    myfree(w, M_TASK);
	    if (!waiters)
		network_unregister_fd(0);
	}
	if (tea != TEA_CONTINUE)
	    return tea;
    }

    return TEA_CONTINUE;
}

static void
stdin_readable(int fd, void *data)
{
    char buffer[1000];
    int n;
    Var v;
    stdin_waiter *w;

    if (data != &waiters)
	panic("STDIN_READABLE: Bad data!");

    if (!waiters) {
	errlog("STDIN_READABLE: Nobody cares!\n");
	return;
    }
    n = read(0, buffer, sizeof(buffer));
    buffer[n] = '\0';
    while (n)
	if (buffer[--n] == '\n')
	    buffer[n] = 'X';

    if (buffer[0] == 'a') {
	v.type = TYPE_ERR;
	v.v.err = E_NACC;
    } else {
	v.type = TYPE_STR;
	v.v.str = str_dup(buffer);
    }

    resume_task(waiters->the_vm, v);
    w = waiters->next;
    myfree(waiters, M_TASK);
    waiters = w;
    if (!waiters)
	network_unregister_fd(0);
}

static enum error
stdin_suspender(vm the_vm, void *data)
{
    stdin_waiter *w = data;

    if (!waiters)
	network_register_fd(0, stdin_readable, 0, &waiters);

    w->the_vm = the_vm;
    w->next = waiters;
    waiters = w;

    return E_NONE;
}

static package
bf_read_stdin(Var arglist, Byte next, void *vdata, Objid progr)
{
    stdin_waiter *w =
	(stdin_waiter *) mymalloc(sizeof(stdin_waiter), M_TASK);

    return make_suspend_pack(stdin_suspender, w);
}
#endif				/* EXAMPLE */

#define STUPID_VERB_CACHE 1
#ifdef STUPID_VERB_CACHE
#include "utils.h"

static package
bf_verb_cache_stats(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var r;

    free_var(arglist);

    if (!is_wizard(progr)) {
	return make_error_pack(E_PERM);
    }
    r = db_verb_cache_stats();

    return make_var_pack(r);
}

static package
bf_log_cache_stats(Var arglist, Byte next, void *vdata, Objid progr)
{
    free_var(arglist);

    if (!is_wizard(progr)) {
	return make_error_pack(E_PERM);
    }
    db_log_cache_stats();

    return no_var_pack();
}
#endif

static package
bf_isa(Var arglist, Byte next, void *vdata, Objid progr)
{
    Objid what = arglist.v.list[1].v.obj;
    Objid targ = arglist.v.list[2].v.obj;
    Var r;

    free_var(arglist);

    r.type = TYPE_INT;

    while (valid(what)) {
	if (what == targ) {
	    r.v.num = 1;
	    return make_var_pack(r);
	}

	what = db_object_parent(what);
    }

    r.v.num = 0;
    return make_var_pack(r);
}

// Begin VERYRANDOM code from tiresias

static unsigned long regA, regB, regC;

//int VERYRANDOM(unsigned long &regA, unsigned long &regB, unsigned long &regC) {
int
VERYRANDOM()
{
    regA =
	((((regA >> 31) ^ (regA >> 6) ^ (regA >> 4) ^ (regA >> 2) ^
	   (regA << 1) ^ regA) & 0x00000001) << 31) | (regA >> 1);
    regB =
	((((regB >> 30) ^ (regB >> 2)) & 0x00000001) << 30) | (regB >> 1);
    regC =
	((((regC >> 28) ^ (regC >> 1)) & 0x00000001) << 28) | (regC >> 1);

    return ((regA ^ regB ^ regC) & 0x00000001);
//  return ((regA & regB) | (!regA & regC)) & 0x00000001;
}

static package
bf_vrandomseed(Var arglist, Byte next, void *vdata, Objid progr)
{
    int nargs = arglist.v.list[0].v.num;
    Var r;

    free_var(arglist);

    if (nargs != 0) {
	if (nargs != 1 || arglist.v.list[1].v.list[0].v.num != 3) {
	    return make_error_pack(E_INVARG);
	} else if (arglist.v.list[1].v.list[1].type != TYPE_FLOAT
		   || arglist.v.list[1].v.list[2].type != TYPE_FLOAT
		   || arglist.v.list[1].v.list[3].type != TYPE_FLOAT) {
	    return make_error_pack(E_INVARG);
	} else {
	    unsigned long newRegA =
		(unsigned long) *arglist.v.list[1].v.list[1].v.fnum;
	    unsigned long newRegB =
		(unsigned long) *arglist.v.list[1].v.list[2].v.fnum;
	    unsigned long newRegC =
		(unsigned long) *arglist.v.list[1].v.list[3].v.fnum;

	    regA = newRegA;
	    regB = newRegB;
	    regC = newRegC;
	}
    }

    r = new_list(3);
    r.v.list[1].type = TYPE_FLOAT;
    r.v.list[1].v.fnum = (double *) mymalloc(sizeof(double), M_FLOAT);
    *r.v.list[1].v.fnum = (float) regA;

    r.v.list[2].type = TYPE_FLOAT;
    r.v.list[2].v.fnum = (double *) mymalloc(sizeof(double), M_FLOAT);
    *r.v.list[2].v.fnum = (float) regB;

    r.v.list[3].type = TYPE_FLOAT;
    r.v.list[3].v.fnum = (double *) mymalloc(sizeof(double), M_FLOAT);
    *r.v.list[3].v.fnum = (float) regC;

    return make_var_pack(r);
}

static package
bf_vrandom(Var arglist, Byte next, void *vdata, Objid progr)
{
    int nargs = arglist.v.list[0].v.num;
    int num = (nargs >= 1 ? arglist.v.list[1].v.num : 1);

    free_var(arglist);

    if (num <= 0)
	return make_error_pack(E_INVARG);
    else {
	Var r;
	int bits = 0;
	int result = -1;

	r.type = TYPE_INT;
	if (nargs == 0)
	    bits = 31;
	else {
	    double x = pow(num, .5);
	    bits =
		(int) ((double) abs((int) x) == x ? x : abs((int) x + 1));
	}

	while (result < 1 || result > num) {
	    int x = 0;
	    result = 1;
	    for (x = 0; x < bits; x++) {
		int rbit, powwow;
		powwow = (int) pow(2, x);
//              rbit = VERYRANDOM(regA, regB, regC);
		rbit = VERYRANDOM();
		result = result + (rbit * powwow);
	    }
	}

	r.v.num = result;
	return make_var_pack(r);
    }
}

// end VERYRANDOM code

// begin utime - andy
static package
bf_utime(Var arglist, Byte next, void *vdata, Objid progr)
{
    double t;
    struct timeval tv;

    free_var(arglist);

    if (gettimeofday(&tv, NULL))
	panic("BF_UTIME: gettimeofday() failed");

    t = tv.tv_sec + (tv.tv_usec / (double) 1000000.0);

    return make_var_pack(new_float(t));
}

// end utime - andy

#ifdef HAVE_EXPAT_H
extern void register_xml(void);
#endif


#ifdef USE_EXT_FUP
extern void register_files(void);
#endif


void
register_extensions()
{
    oklog("          LOADING: extensions ...\n");
#if EXAMPLE
    register_task_queue(stdin_enumerator);
    register_function("read_stdin", 0, 0, bf_read_stdin);
#endif
#ifdef STUPID_VERB_CACHE
    register_function("log_cache_stats", 0, 0, bf_log_cache_stats);
    register_function("verb_cache_stats", 0, 0, bf_verb_cache_stats);
#endif

#ifdef HAVE_EXPAT_H
    register_xml();
#endif

    register_function("isa", 2, 2, bf_isa, TYPE_OBJ, TYPE_OBJ);
    register_function("vrandomseed", 0, 3, bf_vrandomseed, TYPE_LIST);
    register_function("vrandom", 0, 1, bf_vrandom, TYPE_INT);
// begin utime - andy
    register_function("utime", 0, 0, bf_utime);
// end utime - andy

#ifdef USE_EXT_FUP
    register_files();
#endif

    oklog("          LOADING: extensions ... finished\n");
}

