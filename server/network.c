/******************************************************************************
  Copyright (c) 1992 Xerox Corporation.  All rights reserved.
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

#include "options.h"

#if NETWORK_PROTOCOL == NP_SINGLE
#  include "net_single.c"
#else
#  include "net_multi.c"
#endif

Var
network_connection_options(network_handle nh, Var list)
{
    do {
	{
	    Var pair = new_list(2);
	    pair.v.list[1].type = (var_type) (_TYPE_STR | 0x80);
	    pair.v.list[1].v.str = str_dup("client-echo");
	    pair.v.list[2].type = (TYPE_INT);
	    pair.v.list[2].v.num = (((nhandle *) (nh).ptr)->client_echo);
	    (list) = listappend((list), pair);

	    pair = new_list(2);
	    pair.v.list[1].type = TYPE_STR;
	    pair.v.list[1].v.str = str_dup("user-client");
	    pair.v.list[2].type = TYPE_INT;
	    pair.v.list[2].v.num = (((nhandle *) (nh).ptr)->user_client);
	    list = listappend(list, pair);

	    pair = new_list(2);
	    pair.v.list[1].type = TYPE_STR;
	    pair.v.list[1].v.str = str_dup("connect_host");
	    pair.v.list[2].type = TYPE_STR;
	    pair.v.list[2].v.str =
		(((nhandle *) (nh).ptr)->
		 connect_host) ? str_ref((((nhandle *) (nh).ptr)->
					  connect_host)) : str_dup("");
	    list = listappend(list, pair);
	}
	return (list);
    } while (0);
}

int
network_connection_option(network_handle nh, const char *option,
			  Var * value)
{
    do {
	if (!mystrcasecmp((option), "client-echo")) {
	    (value)->type = (TYPE_INT);
	    (value)->v.num = (((nhandle *) (nh).ptr)->client_echo);
	    return 1;
	}
	if (!mystrcasecmp(option, "user-client")) {
	    (value)->type = TYPE_INT;
	    (value)->v.num = (((nhandle *) (nh).ptr)->user_client);
	    return 1;
	}
	if (!mystrcasecmp(option, "connect_host")) {
	    (value)->type = TYPE_STR;
	    (value)->v.str =
		(((nhandle *) (nh).ptr)->
		 connect_host) ? str_ref((((nhandle *) (nh).ptr)->
					  connect_host)) : str_dup("");
	    return 1;
	}
	return 0;
    } while (0);
}

int
network_set_connection_option(network_handle nh, const char *option,
			      Var value)
{
    do {
	if (!mystrcasecmp((option), "client-echo")) {
	    network_set_client_echo((nh), is_true((value)));;
	    return 1;
	}
	if (!mystrcasecmp(option, "user-client")) {
	    (((nhandle *) (nh).ptr)->user_client) = value.v.num;
	    return 1;
	}
	if (!mystrcasecmp(option, "connect_host")) {
	    if (value.type == TYPE_STR && value.v.str[0] != '\0')
		(((nhandle *) (nh).ptr)->connect_host) =
		    str_dup(value.v.str);
	    else
		(((nhandle *) (nh).ptr)->connect_host) = 0;
	    return 1;
	}
	return 0;
    } while (0);
}

/*
 * after macro expansion, g++ complained that:

g++ -O  -c -o network.o network.c
network.c: In function 'Var network_connection_options(network_handle, Var)':
network.c:29: error: invalid conversion from 'int' to 'var_type'
make: *** [network.o] Error 1

* so I'm commenting this out and placing the macro-expanded copy above.

Var
network_connection_options(network_handle nh, Var list)
{
    CONNECTION_OPTION_LIST(NETWORK_CO_TABLE, nh, list);
}

int
network_connection_option(network_handle nh, const char *option, Var * value)
{
    CONNECTION_OPTION_GET(NETWORK_CO_TABLE, nh, option, value);
}

int
network_set_connection_option(network_handle nh, const char *option, Var value)
{
    CONNECTION_OPTION_SET(NETWORK_CO_TABLE, nh, option, value);
}
*/

