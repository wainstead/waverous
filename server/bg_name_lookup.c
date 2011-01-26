#include <errno.h>
#include <signal.h>
#include <netdb.h>		/* gethostby{name,addr}() */
#include <unistd.h>		/* write(), read(), sleep(), alarm() */
#include <stdlib.h>		/* malloc(), free() */
#include <netinet/in.h>		/* inet_ntoa() */
#include <arpa/inet.h>		/* ditto */
#include <time.h>

#include "tasks.h"
#include "structures.h"
#include "functions.h"
#include "utils.h"
#include "list.h"
#include "my-in.h"

#include "log.h"		/* errlog() */
#include "exceptions.h"		/* panic() */
#include "net_multi.h"		/* network_{,un}register_fd() */
#include "storage.h"		/* str_dup() */
#include "server.h"		/* set_server_cmdline() */

enum lookup_enum { ASYNC_FROM_ADDR, ASYNC_FROM_NAME };
struct async_name_lookup_info {
    enum lookup_enum kind;

    unsigned timeout;
    unsigned started;

    union {
	char *name;
	struct in_addr address;
    } u;

    pid_t pid;
    int pipe_fd;

    vm the_vm;

    struct async_name_lookup_info *next;
};

enum request_enum { REQ_FROM_ADDR, REQ_FROM_NAME };
struct async_request {		/* Same as name_lookup.c's struct request */
    enum request_enum kind;

    unsigned timeout;

    union {
	unsigned length;
	/* struct sockaddr_in address; Why? */
	struct in_addr address;
    } u;
};

enum reply_enum { AR_SUCCESS, AR_FAILURE };
struct async_reply {
    enum reply_enum success;

    enum error error;

    int num_aliases;
    int num_addresses;
};

typedef struct async_reply async_reply;
typedef struct async_request async_request;

struct async_name_lookup_info *async_name_lookup_info = 0;
int async_name_lookup_len = 0;

static task_enum_action lookup_task_queue_enumerator(task_closure closure,
						     void *data);
static void async_name_lookup_readable(int fd, void *ip);
static struct async_name_lookup_info *dequeue_lookup_task(struct
							  async_name_lookup_info
							  *info_ptr);

static struct async_name_lookup_info *async_lookup_from_name(const char
							     *name,
							     unsigned
							     timeout);
static struct async_name_lookup_info *async_lookup_from_addr(struct in_addr
							     addr,
							     unsigned
							     timeout);

static void async_lookup_from_addr_child(int to_parent, int from_parent);
static void async_lookup_from_name_child(int to_parent, int from_parent);

static void async_child_catch_alarm(int sig);

/*** Utils ***/

static ssize_t			/* Shamelessly stolen from name_lookup.c */
robust_read(int fd, void *buffer, int len)
{
    int count;

    do {
	count = read(fd, buffer, len);
    } while (count == -1 && errno == EINTR);

    return count;
}

static inline ssize_t
robust_write(int fd, const void *buffer, int len)
{
    int ret;

    while (((ret = write(fd, buffer, len)) == -1) && (errno == EINTR));

    return ret;
}

/* Writes the string length followed by the string to an fd.
 */
static inline int
string_write(int fd, char *str)
{
    ssize_t ret;
    size_t size;

    size = strlen(str);

    ret = robust_write(fd, &size, sizeof(size));
    if (ret != sizeof(size))
	return ret;

    ret = robust_write(fd, str, size);
    return ret;
}

/* Reads a string length followed by a string.
 * Returns NULL on error. Otherwise caller should free().
 */
static inline char *
string_read(int fd)
{
    ssize_t ret;
    size_t size;
    char *str;

    ret = robust_read(fd, &size, sizeof(size));
    if (ret != sizeof(size))
	return NULL;

    str = (char *) malloc(size + 1);
    if (str == NULL)
	panic("STRING_READ: Out of memory.\n");

    ret = robust_read(fd, str, size);
    if (ret != (ssize_t) size) {
	free(str);
	return NULL;
    }

    str[size] = '\0';
    return str;
}


/* Task queue stuff */


/* Enumerator for custom task queue. Target of register_task_queue().
 */
static task_enum_action
lookup_task_queue_enumerator(task_closure closure, void *data)
{
    int x;
    struct async_name_lookup_info *info_ptr;
    task_enum_action tea;
    static const char status[] = "name_lookup()";

    info_ptr = async_name_lookup_info;

    for (x = 1; x <= async_name_lookup_len; x++) {
	if (!info_ptr)
	    panic
		("LOOKUP_TASK_QUEUE_ENUMERATOR: Ran off end of linked list!");

	tea = (*closure) (info_ptr->the_vm, status, data);

	if (tea == TEA_KILL) {
	    errlog("LOOKUP_TASK_QUEUE_ENUMERATOR: TEA_KILL\n");
	    dequeue_lookup_task(info_ptr);
	}

	if (tea != TEA_CONTINUE)
	    return tea;

	info_ptr = info_ptr->next;
    }

    return TEA_CONTINUE;
}


static enum error
enqueue_lookup_task(vm the_vm, void *ip)
{
    struct async_name_lookup_info **info_walk;

    struct async_name_lookup_info *info_ptr =
	(struct async_name_lookup_info *) ip;

    network_register_fd(info_ptr->pipe_fd, async_name_lookup_readable,
			NULL, info_ptr);
    info_ptr->the_vm = the_vm;

    /* Easier to throw them on the beginning, but oh well */
    info_walk = &async_name_lookup_info;
    while (*info_walk) {
	info_walk = &((*info_walk)->next);
    }

    *info_walk = info_ptr;

    async_name_lookup_len++;

    return E_NONE;
}


/* Scans the name lookup queue for the pointer we are given, and deletes it
 * from the list. Returns a pointer to the next item in the list.
 */
static struct async_name_lookup_info *
dequeue_lookup_task(struct async_name_lookup_info *info_ptr)
{
    struct async_name_lookup_info **info_walk;
    int x = 0;
    int cleaned = 0;

    network_unregister_fd(info_ptr->pipe_fd);
    close(info_ptr->pipe_fd);

    for (info_walk = &async_name_lookup_info; *info_walk;
	 info_walk = &((*info_walk)->next)) {
	if (*info_walk == info_ptr) {
	    struct async_name_lookup_info *next;

	    next = (*info_walk)->next;
	    free(*info_walk);
	    *info_walk = next;
	    cleaned = 1;
	    break;
	}

	if (++x > async_name_lookup_len)
	    panic
		("DEQUEUE_LOOKUP_TASK: Queue is longer than it should be. :-(");
    }

    if (!cleaned)
	panic("DEQUEUE_LOOKUP_TASK: Failed to clean up.");

    async_name_lookup_len--;

    return *info_walk;
}


/* Callback set in place with network_register_fd().
 */
static void
async_name_lookup_readable(int fd, void *ip)
{
    struct async_name_lookup_info *info_ptr =
	(struct async_name_lookup_info *) ip;

    async_reply rep;
    Var r;

    char *name;

    if ((robust_read(info_ptr->pipe_fd, &rep, sizeof(rep)) != sizeof(rep))) {
	/* fd was probably closed as child died */
	r.type = TYPE_ERR;
	r.v.err = E_VARNF;
    } else if (rep.success == AR_FAILURE) {
	/* Child reported an error, just return that */
	r.type = TYPE_ERR;
	r.v.err = rep.error;
    } else if ((name = string_read(fd)) == NULL) {
	/* fd was probably closed as child died */
	r.type = TYPE_ERR;
	r.v.err = E_VARNF;
    } else {
	int x;
	char **alias_list;
	struct in_addr *addr_list;

	if ((alias_list =
	     (char **) calloc(rep.num_aliases,
			      sizeof(alias_list[0]))) == NULL) {
	    panic("ASYNC_NAME_LOOKUP_READABLE: Out of memory\n");
	}

	if ((addr_list =
	     (struct in_addr *) calloc(rep.num_addresses,
				       sizeof(struct in_addr))) == NULL) {
	    panic("ASYNC_NAME_LOOKUP_READABLE: Out of memory\n");
	}

	/* Read the alias ptrs into our array */
	for (x = 0; x < rep.num_aliases; x++) {
	    alias_list[x] = string_read(info_ptr->pipe_fd);
	    /* oklog("Alias %d: %s\n", x, alias_list[x]); */
	    /* Should do something smarter on error */
	    if (alias_list[x] == NULL)
		panic
		    ("ASYNC_NAME_LOOKUP_READABLE: Couldn't read alias.\n");
	}

	/* Read the addrs */
	for (x = 0; x < rep.num_addresses; x++) {
	    /* Should do something smarter on error */
	    if (robust_read
		(info_ptr->pipe_fd, &(addr_list[x].s_addr),
		 sizeof(addr_list[0].s_addr)) !=
		sizeof(addr_list[0].s_addr))
		panic
		    ("ASYNC_NAME_LOOKUP_READABLE: Couldn't read address.\n");
	    /* oklog("Address %d: %s\n", x, inet_ntoa(addr_list[x])); */
	}

	/* Yay, return successful result */
	r = new_list(3);

	r.v.list[1].type = TYPE_STR;
	r.v.list[1].v.str = str_dup(name);

	r.v.list[2] = new_list(rep.num_aliases);
	for (x = 0; x < rep.num_aliases; x++) {
	    r.v.list[2].v.list[x + 1].type = TYPE_STR;
	    r.v.list[2].v.list[x + 1].v.str = str_dup(alias_list[x]);
	}

	r.v.list[3] = new_list(rep.num_addresses);
	for (x = 0; x < rep.num_addresses; x++) {
	    unsigned char *y = (unsigned char *) &(addr_list[x].s_addr);

	    r.v.list[3].v.list[x + 1] = new_list(4);
	    r.v.list[3].v.list[x + 1].v.list[1].type = TYPE_INT;
	    r.v.list[3].v.list[x + 1].v.list[1].v.num = (int) *y++;
	    r.v.list[3].v.list[x + 1].v.list[2].type = TYPE_INT;
	    r.v.list[3].v.list[x + 1].v.list[2].v.num = (int) *y++;
	    r.v.list[3].v.list[x + 1].v.list[3].type = TYPE_INT;
	    r.v.list[3].v.list[x + 1].v.list[3].v.num = (int) *y++;
	    r.v.list[3].v.list[x + 1].v.list[4].type = TYPE_INT;
	    r.v.list[3].v.list[x + 1].v.list[4].v.num = (int) *y++;
	}

	/* Free all our stuff */
	free(name);
	free(alias_list);
	free(addr_list);
    }

    resume_task(info_ptr->the_vm, r);

    /* Clean up */
    dequeue_lookup_task(info_ptr);

    return;
}


/*** Builtin function stuff ***/


/* bf_name_lookup(str/list addr, int timeout) => {str name, list aliases, list addresses}
 * E_PERM    - Not a wizard.
 * E_RECMOVE - Problem starting name lookup.
 * E_VARNF   - Name lookup died. (Timeout)
 * Results from gethostbyname():
 * E_PROPNF  - Host was not found.
 * E_MAXREC  - Temporary error, try again.
 * E_INVIND  - Host exists but has no address information.
 * E_VERBNF  - Unrecoverable error.
 */
static package
bf_name_lookup(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var r;			/* probably unneeded */
    struct async_name_lookup_info *info_ptr;

    int timeout = arglist.v.list[2].v.num;

    if (!is_wizard(progr)) {
	free_var(arglist);
	return make_error_pack(E_PERM);
    }

    switch (arglist.v.list[1].type) {
    case TYPE_STR:
	if ((info_ptr =
	     async_lookup_from_name(arglist.v.list[1].v.str,
				    timeout)) == NULL) {
	    free_var(arglist);
	    return make_error_pack(E_RECMOVE);	/* Problem starting or talking to child */
	} else {
	    free_var(arglist);
	    return make_suspend_pack(enqueue_lookup_task, info_ptr);
	}

	break;
    case TYPE_LIST:
	if (arglist.v.list[1].v.list[0].v.num != 4) {
	    free_var(arglist);
	    return make_error_pack(E_INVARG);
	} else {
	    int x;
	    struct in_addr tmp_addr;
	    unsigned char *y = (unsigned char *) &(tmp_addr.s_addr);

	    /* We have a list of length 4. Turn it into an IP. */
	    for (x = 1; x <= 4; x++) {
		if ((arglist.v.list[1].v.list[x].type != TYPE_INT) ||
		    (arglist.v.list[1].v.list[x].v.num < 0) ||
		    (arglist.v.list[1].v.list[x].v.num > 255)) {
		    free_var(arglist);
		    return make_error_pack(E_INVARG);
		} else
		    *y++ =
			(unsigned char) arglist.v.list[1].v.list[x].v.num;
	    }

	    free_var(arglist);	/* We're done with them at this point */

	    /* Try to look it up. */
	    if ((info_ptr =
		 async_lookup_from_addr(tmp_addr, timeout)) == NULL)
		return make_error_pack(E_RECMOVE);	/* Problem starting or talking to child */
	    else
		return make_suspend_pack(enqueue_lookup_task, info_ptr);
	}

	break;
    default:
	free_var(arglist);
	return make_error_pack(E_TYPE);
	break;
    }

    /* Hmm... */
    errlog("BF_NAME_LOOKUP: How did we get here?\n");

    r.type = TYPE_INT;
    r.v.num = 0;

    free_var(arglist);
    return make_var_pack(r);
}



/* bf_name_lookups() => {{str/list addr, int timeout, int started, int task_id}, ...}
 */
static package
bf_name_lookups(Var arglist, Byte next, void *vdata, Objid progr)
{
    Var r;
    int x;
    struct async_name_lookup_info *info_ptr;

    if (!is_wizard(progr)) {
	free_var(arglist);
	return make_error_pack(E_PERM);
    }

    info_ptr = async_name_lookup_info;
    r = new_list(async_name_lookup_len);

    for (x = 1; x <= async_name_lookup_len; x++) {
	r.v.list[x] = new_list(4);

	if (!info_ptr)
	    panic("BF_NAME_LOOKUPS: Ran off end of linked list!");

	r.v.list[x].v.list[1].type = TYPE_STR;
	r.v.list[x].v.list[1].v.str = str_dup(info_ptr->u.name);

	r.v.list[x].v.list[2].type = TYPE_INT;
	r.v.list[x].v.list[2].v.num = info_ptr->timeout;

	r.v.list[x].v.list[3].type = TYPE_INT;
	r.v.list[x].v.list[3].v.num = info_ptr->started;

	r.v.list[x].v.list[4].type = TYPE_INT;
	r.v.list[x].v.list[4].v.num = info_ptr->the_vm->task_id;

	info_ptr = info_ptr->next;
    }

    free_var(arglist);
    return make_var_pack(r);
}


void
register_name_lookup()
{
    register_task_queue(lookup_task_queue_enumerator);
    (void) register_function("name_lookup", 2, 2, bf_name_lookup, TYPE_ANY,
			     TYPE_INT);
    (void) register_function("name_lookups", 0, 0, bf_name_lookups);
}


/*** Interface with child stuff ***/


/* Hmm.
 * We probably inherit signal handlers?
 * And we should probably close() the network fd's?
 */
static pid_t
spawn_async_lookup(void (*child_proc) (int to_parent, int from_parent),
		   int *to_child, int *from_child)
{
    int pipe_to_child[2], pipe_from_child[2];
    pid_t pid;

    if (pipe(pipe_to_child) < 0) {
	log_perror("SPAWN_ASYNC_LOOKUP: Couldn't create first pipe");
    } else if (pipe(pipe_from_child) < 0) {
	log_perror("SPAWN_ASYNC_LOOKUP: Couldn't create second pipe");
	close(pipe_to_child[0]);
	close(pipe_to_child[1]);
    } else if ((pid = fork()) < 0) {
	log_perror
	    ("SPAWN_ASYNC_LOOKUP: Couldn't fork name lookup process");
	close(pipe_to_child[0]);
	close(pipe_to_child[1]);
	close(pipe_from_child[0]);
	close(pipe_from_child[1]);
    } else if (pid != 0) {	/* parent */

	close(pipe_to_child[0]);
	close(pipe_from_child[1]);
	*to_child = pipe_to_child[1];
	*from_child = pipe_from_child[0];

	return pid;
    } else {			/* child */
	close(pipe_to_child[1]);
	close(pipe_from_child[0]);

	signal(SIGALRM, async_child_catch_alarm);

	(*child_proc) (pipe_from_child[1], pipe_to_child[0]);
	exit(0);
    }

    return 0;
}


static struct async_name_lookup_info *
async_lookup_from_name(const char *name, unsigned timeout)
{
    struct async_name_lookup_info *info_new;

    async_request req;

    int pid;
    int to_child, from_child;


    if ((info_new =
	 (struct async_name_lookup_info *)
	 malloc(sizeof(struct async_name_lookup_info))) == NULL) {
	errlog("ASYNC_LOOKUP_FROM_NAME: Malloc failed.\n");
	return NULL;
    }

    pid =
	spawn_async_lookup(async_lookup_from_name_child, &to_child,
			   &from_child);
    if (!pid) {
	errlog("ASYNC_LOOKUP_FROM_NAME: Could not spawn child!\n");
	free(info_new);
	return NULL;
    }

    req.kind = REQ_FROM_NAME;
    req.timeout = timeout;
    req.u.length = strlen(name);

    if (robust_write(to_child, &req, sizeof(req)) == -1) {
	log_perror
	    ("ASYNC_LOOKUP_FROM_NAME: Couldn't write request to child");
	free(info_new);
	return NULL;
    }

    if (robust_write(to_child, name, req.u.length) == -1) {
	log_perror("ASYNC_LOOKUP_FROM_NAME: Couldn't write name to child");
	free(info_new);
	return NULL;
    }

    close(to_child);

    info_new->timeout = timeout;
    info_new->started = time(NULL);
    info_new->u.name = str_dup(name);
    info_new->pid = pid;
    info_new->pipe_fd = from_child;
    info_new->next = 0;

    return info_new;
}


static struct async_name_lookup_info *
async_lookup_from_addr(struct in_addr addr, unsigned timeout)
{
    struct async_name_lookup_info *info_new;

    async_request req;

    int pid;
    int to_child, from_child;


    if ((info_new =
	 (struct async_name_lookup_info *)
	 malloc(sizeof(struct async_name_lookup_info))) == NULL) {
	errlog("ASYNC_LOOKUP_FROM_ADDR: Malloc failed.\n");
	return NULL;
    }

    pid =
	spawn_async_lookup(async_lookup_from_addr_child, &to_child,
			   &from_child);
    if (!pid) {
	errlog("ASYNC_LOOKUP_FROM_ADDR: Could not spawn child!\n");
	free(info_new);
	return NULL;
    }

    req.kind = REQ_FROM_ADDR;
    req.timeout = timeout;
    req.u.address.s_addr = addr.s_addr;

    if (robust_write(to_child, &req, sizeof(req)) == -1) {
	log_perror
	    ("ASYNC_LOOKUP_FROM_ADDR: Couldn't write request to child");
	free(info_new);
	return NULL;
    }

    close(to_child);

    info_new->timeout = timeout;
    info_new->started = time(NULL);
    info_new->u.address.s_addr = addr.s_addr;
    info_new->pid = pid;
    info_new->pipe_fd = from_child;
    info_new->next = 0;

    return info_new;
}


/*** Runs in child ***/

/* Name lookup process timed out. Give up.
 */
static void
async_child_catch_alarm(int sig)
{
    _exit(1);
}


/* Called from async_lookup_from_{name,addr}_child.
 * Figures out what our name lookup answer was, and tells the parent about it.
 */
static void
process_lookup_answer(int to_parent, struct hostent *answer)
{
    async_reply rep;

    if (answer == NULL) {
	/* Report failure */
	rep.success = AR_FAILURE;

	switch (h_errno) {
	case HOST_NOT_FOUND:
	    rep.error = E_PROPNF;
	    break;
	case TRY_AGAIN:
	    rep.error = E_MAXREC;
	    break;
	case NO_DATA:
	    rep.error = E_INVIND;
	    break;
	default:
	    rep.error = E_VERBNF;
	    break;
	}

	write(to_parent, &rep, sizeof(rep));

	_exit(1);
    } else {
	int x;

	char **addr_list = answer->h_addr_list;
	char **alias_list = answer->h_aliases;


	/* Report success */
	rep.success = AR_SUCCESS;
	rep.error = E_NONE;

	/* Count the number of aliases */
	for (x = 0; alias_list[x]; x++);
	rep.num_aliases = x;

	/* Count the number of addresses */
	for (x = 0; addr_list[x]; x++);
	rep.num_addresses = x;

	/* sleep(10); */
	robust_write(to_parent, &rep, sizeof(rep));

	/* Write the CNAME */
	string_write(to_parent, answer->h_name);

	/* Write the list of aliases */
	for (x = 0; x < rep.num_aliases; x++)
	    string_write(to_parent, alias_list[x]);

	/* Write the list of addresses */
	for (x = 0; x < rep.num_addresses; x++)
	    robust_write(to_parent, addr_list[x], answer->h_length);
    }

}


static void
async_lookup_from_name_child(int to_parent, int from_parent)
{
    async_request req;
    char *name, *cmdline;
    struct hostent *answer;

    if (robust_read(from_parent, &req, sizeof(req)) != sizeof(req)) {
	errlog("ASYNC_LOOKUP_FROM_NAME_CHILD: Read of request failed.\n");
	_exit(1);
    } else if (req.kind != REQ_FROM_NAME) {
	errlog("ASYNC_LOOKUP_FROM_NAME_CHILD: Unexpected request kind.\n");
	_exit(1);
    } else if ((name = (char *) malloc(req.u.length + 1)) == NULL) {
	errlog("ASYNC_LOOKUP_FROM_NAME_CHILD: Out of memory\n");
	_exit(1);
    } else if (robust_read(from_parent, name, req.u.length) !=
	       (ssize_t) req.u.length) {
	errlog("ASYNC_LOOKUP_FROM_NAME_CHILD: Read of name failed.\n");
	_exit(1);
    }

    name[req.u.length] = '\0';

    /* Set the command line while we're doing our lookup */
    cmdline = (char *) malloc(50 + req.u.length);
    if (cmdline != NULL) {
	sprintf(cmdline, "(MOO async name lookup: %s)", name);
	set_server_cmdline(cmdline);
	free(cmdline);
    }

    alarm(req.timeout);

    answer = gethostbyname(name);

    alarm(0);

    process_lookup_answer(to_parent, answer);

    return;
}


static void
async_lookup_from_addr_child(int to_parent, int from_parent)
{
    async_request req;
    char cmdline[50];
    struct hostent *answer;

    if (robust_read(from_parent, &req, sizeof(req)) != sizeof(req)) {
	errlog("ASYNC_LOOKUP_FROM_ADDR_CHILD: Read of request failed.\n");
	_exit(1);
    } else if (req.kind != REQ_FROM_ADDR) {
	errlog("ASYNC_LOOKUP_FROM_ADDR_CHILD: Unexpected request kind.\n");
	_exit(1);
    }

    /* Set the command line while we're doing our lookup */
    sprintf(cmdline, "(MOO async name lookup: %s)",
	    inet_ntoa(req.u.address));
    set_server_cmdline(cmdline);

    alarm(req.timeout);

    answer =
	gethostbyaddr((void *) &(req.u.address.s_addr),
		      sizeof(req.u.address.s_addr), AF_INET);

    alarm(0);

    process_lookup_answer(to_parent, answer);

    return;
}
