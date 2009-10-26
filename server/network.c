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
      pair.v.list[1].type = (var_type)(_TYPE_STR | 0x80);
      pair.v.list[1].v.str = str_dup("client-echo");
      pair.v.list[2].type = (TYPE_INT);
      pair.v.list[2].v.num = (((nhandle *)(nh).ptr)->client_echo);
      (list) = listappend((list), pair);

      pair = new_list(2);
      pair.v.list[1].type = TYPE_STR;
      pair.v.list[1].v.str = str_dup("user-client");
      pair.v.list[2].type = TYPE_INT; 
      pair.v.list[2].v.num = (((nhandle *)(nh).ptr)->user_client);
      list = listappend(list, pair);

      pair = new_list(2);
      pair.v.list[1].type = TYPE_STR;
      pair.v.list[1].v.str = str_dup("connect_host");
      pair.v.list[2].type = TYPE_STR; 
      pair.v.list[2].v.str = (((nhandle *)(nh).ptr)->connect_host) ? str_ref((((nhandle *)(nh).ptr)->connect_host)) : str_dup("");
      list = listappend(list, pair);
    } return (list);
  } while (0);
}

int
network_connection_option(network_handle nh, const char *option, Var * value)
{
  do { if (!mystrcasecmp((option), "client-echo")) { (value)->type = (TYPE_INT);
      (value)->v.num = (((nhandle *)(nh).ptr)->client_echo);
      return 1;
    } 
    if (!mystrcasecmp(option, "user-client")) {
        (value)->type = TYPE_INT;
        (value)->v.num = (((nhandle *)(nh).ptr)->user_client);
        return 1;
    }
    if (!mystrcasecmp(option, "connect_host")) {
        (value)->type = TYPE_STR;
        (value)->v.str = str_ref(((nhandle *)(nh).ptr)->connect_host);
        return 1;
    }
    return 0;
  } while (0);
}

int
network_set_connection_option(network_handle nh, const char *option, Var value)
{
  do { if (!mystrcasecmp((option), "client-echo")) { network_set_client_echo((nh), is_true((value)));;
      return 1;
    }
    if (!mystrcasecmp(option, "user-client")) {
        (((nhandle *)(nh).ptr)->user_client) = value.v.num;
       return 1;
     }
    if (!mystrcasecmp(option, "connect_host")) {
        if (value.type == TYPE_STR && value.v.str[0] != '\0')
          (((nhandle *)(nh).ptr)->connect_host) = str_dup(value.v.str);
        else
          (((nhandle *)(nh).ptr)->connect_host) = 0;
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

/* 
 * $Log: not supported by cvs2svn $
 * Revision 1.2.10.1  2003/06/07 12:59:04  wrog
 * introduced connection_option macros
 *
 * Revision 1.2  1998/12/14 13:18:35  nop
 * Merge UNSAFE_OPTS (ref fixups); fix Log tag placement to fit CVS whims
 *
 * Revision 1.1.1.1  1997/03/03 03:45:00  nop
 * LambdaMOO 1.8.0p5
 *
 * Revision 2.0  1995/11/30  05:11:37  pavel
 * New baseline version, corresponding to release 1.8.0alpha1.
 *
 * Revision 1.5  1992/10/23  23:03:47  pavel
 * Added copyright notice.
 *
 * Revision 1.4  1992/10/21  03:02:35  pavel
 * Converted to use new automatic configuration system.
 *
 * Revision 1.3  1992/09/26  02:29:24  pavel
 * Dyked out useless support plans for XNS protocols.
 *
 * Revision 1.2  1992/09/23  17:18:44  pavel
 * Now supports the new networking architecture, switching among protocol and
 * multiplexing-wait implementations based on settings in config.h.
 *
 * Revision 1.1  1992/09/03  21:09:51  pavel
 * Initial RCS-controlled version.
 */
