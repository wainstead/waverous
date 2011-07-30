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

/* Multi-user networking protocol implementation for TCP/IP on BSD UNIX */

#include "my-inet.h"		/* inet_addr() */
#include <errno.h>		/* EMFILE, EADDRNOTAVAIL, ECONNREFUSED,
				 * ENETUNREACH, ETIMEOUT */
#include "my-in.h"		/* struct sockaddr_in, INADDR_ANY, htons(),
				 * htonl(), ntohl(), struct in_addr */
#include "my-socket.h"		/* socket(), AF_INET, SOCK_STREAM,
				 * setsockopt(), SOL_SOCKET, SO_REUSEADDR,
				 * bind(), struct sockaddr, accept(),
				 * connect() */
#include "my-stdlib.h"		/* strtoul() */
#include "my-string.h"		/* memcpy() */
#include "my-unistd.h"		/* close() */

#include "config.h"
#include "list.h"
#include "log.h"
#include "name_lookup.h"
#include "net_proto.h"
#include "options.h"
#include "server.h"
#include "streams.h"
#include "timers.h"
#include "utils.h"

#include "net_tcp.c"

#include <netdb.h>

const char *
proto_name(void)
{
    return "BSD/TCP";
}

int
proto_initialize(struct proto *proto, Var * desc, int argc, char **argv)
{
    int port = DEFAULT_PORT;

    proto->pocket_size = 1;
    proto->believe_eof = 1;
    proto->eol_out_string = "\r\n";

    if (!tcp_arguments(argc, argv, &port))
	return 0;

    initialize_name_lookup();

    desc->type = TYPE_INT;
    desc->v.num = port;
    return 1;
}

enum error
proto_make_listener(Var desc, int *fd, Var * canon, const char **name)
{
    struct sockaddr_in address;
    int s, port, option = 1;
    static Stream *st = 0;

    if (!st)
	st = new_stream(20);

    if (desc.type != TYPE_INT && desc.type != TYPE_LIST)
	return E_TYPE;

    port = desc.v.num;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	log_perror("Creating listening socket");
	return E_QUOTA;
    }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
		   (char *) &option, sizeof(option)) < 0) {
	log_perror("Setting listening socket options");
	close(s);
	return E_QUOTA;
    }
    memset((char *) &address, 0, sizeof(address));
    address.sin_family = AF_INET;
    if (desc.type == TYPE_INT) {
	port = desc.v.num;
	address.sin_addr.s_addr = bind_local_ip;
    } else {
	unsigned long inaddr;
	inaddr = inet_addr(desc.v.list[1].v.str);
	if (inaddr != INADDR_NONE) {
	    memcpy(&address.sin_addr, &inaddr, sizeof(inaddr));
	} else {
	    struct hostent *hp;
	    hp = gethostbyname(desc.v.list[1].v.str);
	    if (hp == NULL)
		return E_INVARG;

	    memcpy(&address.sin_addr, hp->h_addr, hp->h_length);
	}
	port = desc.v.list[2].v.num;
    }
    address.sin_port = htons(port);
    if (bind(s, (struct sockaddr *) &address, sizeof(address)) < 0) {
	enum error e = E_QUOTA;

	log_perror("Binding listening socket");
	if (errno == EACCES)
	    e = E_PERM;
	close(s);
	return e;
    }
    if (port == 0) {
	socklen_t length = sizeof(address);

	if (getsockname(s, (struct sockaddr *) &address, &length) < 0) {
	    log_perror("Discovering local port number");
	    close(s);
	    return E_QUOTA;
	}
	canon->type = TYPE_INT;
	canon->v.num = ntohs(address.sin_port);
    } else
	*canon = var_ref(desc);

    if (desc.type == TYPE_INT) {
	stream_printf(st, "port %d", canon->v.num);
    } else {
	stream_printf(st, "%s port %d", canon->v.list[1].v.str,
		      canon->v.list[2].v.num);
    }
    *name = reset_stream(st);

    *fd = s;
    return E_NONE;
}

int
proto_listen(int fd)
{
    listen(fd, 5);
    return 1;
}

enum proto_accept_error
proto_accept_connection(int listener_fd, int *read_fd, int *write_fd,
			const char **name)
{
    int fd;
    struct sockaddr_in address;
    socklen_t addr_length = sizeof(address);
    static Stream *s = 0;

    if (!s)
	s = new_stream(100);

    fd = accept(listener_fd, (struct sockaddr *) &address, &addr_length);
    if (fd < 0) {
	if (errno == EMFILE)
	    return PA_FULL;
	else {
	    log_perror("Accepting new network connection");
	    return PA_OTHER;
	}
    }
    *read_fd = *write_fd = fd;
    stream_printf(s, "%s, port %d",
/* bg_name_lookup */
		  inet_ntoa(address.sin_addr),
/* !bg_name_lookup */
		  (int) ntohs(address.sin_port));
    *name = reset_stream(s);
    return PA_OKAY;
}

void
proto_close_connection(int read_fd, int write_fd)
{
    /* read_fd and write_fd are the same, so we only need to deal with one. */
    close(read_fd);
}

void
proto_close_listener(int fd)
{
    close(fd);
}

#ifdef OUTBOUND_NETWORK

#include "exceptions.h"
#include "structures.h"

static Exception timeout_exception;

static void
timeout_proc(Timer_ID id, Timer_Data data)
{
    RAISE(timeout_exception, 0);
}

enum error
proto_open_connection(Var arglist, int *read_fd, int *write_fd,
		      const char **local_name, const char **remote_name)
{
    /* These are `static' rather than `volatile' because I can't cope with
     * getting all those nasty little parameter-passing rules right.  This
     * function isn't recursive anyway, so it doesn't matter.
     */
    static const char *host_name;
    static int port;
    static Timer_ID id;
    socklen_t length;
    int s, result;
    int timeout = server_int_option("outbound_name_lookup_timeout", 5);
    static struct sockaddr_in addr;
    static Stream *st1 = 0, *st2 = 0;

    if (!outbound_network_enabled)
	return E_PERM;

    if (!st1) {
	st1 = new_stream(20);
	st2 = new_stream(50);
    }
    if (arglist.v.list[0].v.num != 2)
	return E_ARGS;
    else if (arglist.v.list[1].type != TYPE_STR ||
	     arglist.v.list[2].type != TYPE_INT)
	return E_TYPE;

    host_name = arglist.v.list[1].v.str;
    port = arglist.v.list[2].v.num;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = lookup_addr_from_name(host_name, timeout);
    if (addr.sin_addr.s_addr == 0)
	return E_INVARG;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	if (errno != EMFILE)
	    log_perror("Making socket in proto_open_connection");
	return E_QUOTA;
    }

    if (bind_local_ip != INADDR_ANY) {
	static struct sockaddr_in local_addr;

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = bind_local_ip;
	local_addr.sin_port = 0;
	/* In theory, if the original listen() succeeded,
	 * then this should too, but who knows, really? */
	if (bind(s, (struct sockaddr *) &local_addr, sizeof(local_addr)) <
	    0) {
	    enum error e = E_QUOTA;

	    log_perror("Binding local address in proto_open_connection");
	    if (errno == EACCES)
		e = E_PERM;
	    close(s);
	    return e;
	}
    }

    {
	ES_CtxBlock ES_ctx;
	volatile ES_Value ES_es = ES_Initialize;

	ES_ctx.nx = 0;
	ES_ctx._finally = 0;
	ES_ctx.link = ES_exceptionStack;
	ES_exceptionStack = &ES_ctx;

	if (setjmp(ES_ctx.jmp) != 0)
	    ES_es = ES_Exception;

	while (1) {
	    if (ES_es == ES_EvalBody) {
		/* TRY body goes here */
		{
		    id = set_timer(server_int_option
				   ("outbound_connect_timeout", 5),
				   timeout_proc, 0);
		    result =
			connect(s, (struct sockaddr *) &addr,
				sizeof(addr));
		    cancel_timer(id);
		}
		/* TRY body or handler goes here */
		if (ES_es == ES_EvalBody)
		    ES_exceptionStack = ES_ctx.link;
		break;
	    }
	    if (ES_es == ES_Initialize) {
		if (ES_ctx.nx >= ES_MaxExceptionsPerScope)
		    panic("Too many EXCEPT clauses!");
		ES_ctx.array[ES_ctx.nx++] = &timeout_exception;
	    } else if (ES_ctx.id == &timeout_exception
		       || &timeout_exception == &ANY) {
		int exception_value = ES_ctx.value;

		ES_exceptionStack = ES_ctx.link;
		exception_value = exception_value;
		/* avoid warnings */
		/* handler goes here */

		{
		    result = -1;
		    errno = ETIMEDOUT;
		    reenable_timers();
		}
		break;
	    }
	    ES_es = ES_EvalBody;
	}
    }

    if (result < 0) {
	close(s);
	if (errno == EADDRNOTAVAIL ||
	    errno == ECONNREFUSED ||
	    errno == ENETUNREACH || errno == ETIMEDOUT)
	    return E_INVARG;
	log_perror("Connecting in proto_open_connection");
	return E_QUOTA;
    }
    length = sizeof(addr);
    if (getsockname(s, (struct sockaddr *) &addr, &length) < 0) {
	close(s);
	log_perror("Getting local name in proto_open_connection");
	return E_QUOTA;
    }
    *read_fd = *write_fd = s;

    stream_printf(st1, "port %d", (int) ntohs(addr.sin_port));
    *local_name = reset_stream(st1);

    stream_printf(st2, "%s, port %d", host_name, port);
    *remote_name = reset_stream(st2);

    return E_NONE;
}
#endif				/* OUTBOUND_NETWORK */

