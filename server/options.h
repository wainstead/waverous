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

#ifndef Options_h
#define Options_h 1

// LOG_COMMANDS and UNFORKED_CHECKPOINTS have been moved into
// config.h.in; they are ./configure options now. See ./configure --help.

/******************************************************************************
 * If OUT_OF_BAND_PREFIX is defined as a non-empty string, then any lines of
 * input from any player that begin with that prefix will bypass both normal
 * command parsing and any pending read()ing task, instead spawning a server
 * task invoking #0:do_out_of_band_command(word-list).  This is intended for
 * use by fancy clients that need to send reliably-understood messages to the
 * server.
 */

#define OUT_OF_BAND_PREFIX "#$#"

/******************************************************************************
 * If OUT_OF_BAND_QUOTE_PREFIX is defined as a non-empty string, then any
 * lines of input from any player that begin with that prefix will be
 * stripped of that prefix and processed normally (whether to be parsed a
 * command or given to a pending read()ing task), even if the resulting line
 * begins with OUT_OF_BAND_PREFIX.  This provides a means of quoting lines
 * that would otherwise spawn #0:do_out_of_band_command tasks
 */

#define OUT_OF_BAND_QUOTE_PREFIX "#$\""

/******************************************************************************
 * The following constants define the execution limits placed on all MOO tasks.
 *
 * DEFAULT_MAX_STACK_DEPTH is the default maximum depth allowed for the MOO
 *	verb-call stack, the maximum number of dynamically-nested calls at any
 *	given time.  If defined in the database and larger than this default,
 *	$server_options.max_stack_depth overrides this default.
 * DEFAULT_FG_TICKS and DEFAULT_BG_TICKS are the default maximum numbers of
 *	`ticks' (basic operations) any task is allowed to use without
 *	suspending.  If defined in the database, $server_options.fg_ticks and
 *	$server_options.bg_ticks override these defaults.
 * DEFAULT_FG_SECONDS and DEFAULT_BG_SECONDS are the default maximum numbers of
 *	real-time seconds any task is allowed to use without suspending.  If
 *	defined in the database, $server_options.fg_seconds and
 *	$server_options.bg_seconds override these defaults.
 *
 * The *FG* constants are used only for `foreground' tasks (those started by
 * either player input or the server's initiative and that have never
 * suspended); the *BG* constants are used only for `background' tasks (forked
 * tasks and those of any kind that have suspended).
 *
 * The values given below are documented in the LambdaMOO Programmer's Manual,
 * so they should either be left as they are or else the manual should be
 * updated.
 */

#define DEFAULT_MAX_STACK_DEPTH	50

#define DEFAULT_FG_TICKS	30000
#define DEFAULT_BG_TICKS	15000

#define DEFAULT_FG_SECONDS	5
#define DEFAULT_BG_SECONDS	3

/******************************************************************************
 * NETWORK_PROTOCOL must be defined as one of the following:
 *
 * NP_SINGLE	The server will accept only one user at a time, communicating
 *		with them using the standard input and output streams of the
 *		server itself.
 * NP_TCP	The server will use TCP/IP protocols, such as are used by the
 *		Internet `telnet' command.
 * NP_LOCAL	The server will use a non-networking mechanism, entirely local
 *		to the machine on which the server is running.  Depending on
 *		the value of NETWORK_STYLE, below, this will be either UNIX-
 *		domain sockets (NS_BSD) or named pipes (NS_SYSV).
 *
 * If NP_TCP is selected, then DEFAULT_PORT is the TCP port number on which the
 * server listens when no port argument is given on the command line.
 *
 * If NP_LOCAL is selected, then DEFAULT_CONNECT_FILE is the name of the UNIX
 * pseudo-file through which the server will listen for connections when no
 * file name is given on the command line.
 */

#define NETWORK_PROTOCOL 	NP_TCP

#define DEFAULT_PORT 		7777
#define DEFAULT_CONNECT_FILE	"/tmp/.MOO-server"

/******************************************************************************
 * If NETWORK_PROTOCOL is not defined as NP_SINGLE, then NETWORK_STYLE must be
 * defined as one of the following:
 *
 * NS_BSD	The server will use implementation techniques appropriate to a
 *		BSD-style UNIX system.
 * NS_SYSV	The server will use implementation techniques appropriate to an
 *		AT&T UNIX System V system.
 */

#define NETWORK_STYLE NS_BSD

/******************************************************************************
 * If NETWORK_PROTOCOL is not defined as NP_SINGLE, then MPLEX_STYLE must be
 * defined as one of the following:
 *
 * MP_SELECT	The server will assume that the select() system call exists.
 * MP_POLL	The server will assume that the poll() system call exists and
 *		that, if NETWORK_PROTOCOL is NP_LOCAL above, it works for
 *		named pipes (FIFOs).
 * MP_FAKE	The server will use a nasty trick that works only if you've
 *		defined NETWORK_PROTOCOL as NP_LOCAL and NETWORK_STYLE as
 *		NS_SYSV above.
 *
 * Usually, it works best to leave MPLEX_STYLE undefined and let the code at
 * the bottom of this file pick the right value.
 */

/* #define MPLEX_STYLE MP_POLL */


/******************************************************************************
OUTBOUND_NETWORK is now a compilation option; see ./configure --help
for details.
 */


/******************************************************************************
 * The following constants define certain aspects of the server's network
 * behavior if NETWORK_PROTOCOL is not defined as NP_SINGLE.
 *
 * MAX_QUEUED_OUTPUT is the maximum number of output characters the server is
 *		     willing to buffer for any given network connection before
 *		     discarding old output to make way for new.  The server
 *		     only discards output after attempting to send as much as
 *		     possible on the connection without blocking.
 * MAX_QUEUED_INPUT is the maximum number of input characters the server is
 *		    willing to buffer from any given network connection before
 *		    it stops reading from the connection at all.  The server
 *		    starts reading from the connection again once most of the
 *		    buffered input is consumed.
 * DEFAULT_CONNECT_TIMEOUT is the default number of seconds an un-logged-in
 *			   connection is allowed to remain idle without being
 *			   forcibly closed by the server; this can be
 *			   overridden by defining the `connect_timeout'
 *			   property on $server_options or on L, for connections
 *			   accepted by a given listener L.
 */

#define MAX_QUEUED_OUTPUT	65536
#define MAX_QUEUED_INPUT	MAX_QUEUED_OUTPUT
#define DEFAULT_CONNECT_TIMEOUT	300

/******************************************************************************
 * On connections that have not been set to binary mode, the server normally
 * discards incoming characters that are not printable ASCII, including
 * backspace (8) and delete(127).  If INPUT_APPLY_BACKSPACE is defined,
 * backspace and delete cause the preceding character (if any) to be removed
 * from the input stream.  (Comment this out to restore pre-1.8.3 behavior)
 */
#define INPUT_APPLY_BACKSPACE

/******************************************************************************
 * The server maintains a cache of the most recently used patterns from calls
 * to the match() and rmatch() built-in functions.  PATTERN_CACHE_SIZE controls
 * how many past patterns are remembered by the server.  Do not set it to a
 * number less than 1.
 */

#define PATTERN_CACHE_SIZE	20

/******************************************************************************
 * If you don't plan on using protecting built-in properties (like
 * .name and .location), define IGNORE_PROP_PROTECTED.  The extra
 * property lookups on every reference to a built-in property are
 * expensive.
 ****************************************************************************** 
 */

#define IGNORE_PROP_PROTECTED

/******************************************************************************
 * The code generator can now recognize situations where the code will not
 * refer to the value of a variable again and generate opcodes that will
 * keep the interpreter from holding references to the value in the runtime
 * environment variable slot.  Before, when doing something like x=f(x), the
 * interpreter was guaranteed to have a reference to the value of x while f()
 * was running, meaning that f() always had to copy x to modify it.  With
 * BYTECODE_REDUCE_REF enabled, f() could be called with the last reference
 * to the value of x.  So for example, x={@x,y} can (if there are no other
 * references to the value of x in variables or properties) just append to
 * x rather than make a copy and append to that.  If it *does* have to copy,
 * the next time (if it's in a loop) it will have the only reference to the
 * copy and then it can take advantage.
 *
 * NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    
 *
 * This option affects the length of certain bytecode sequences.
 * Suspended tasks in a database from a server built with this option
 * are not guaranteed to work with a server built without this option,
 * and vice versa.  It is safe to flip this switch only if there are
 * no suspended tasks in the database you are loading.  (It might work
 * anyway, but hey, it's your database.)  This restriction will be
 * lifted in a future version of the server software.  Consider this
 * option as being BETA QUALITY until then.
 *
 * NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    NOTE WELL    
 *
 ******************************************************************************
 */
/* #define BYTECODE_REDUCE_REF */

#ifdef BYTECODE_REDUCE_REF
#error Think carefully before enabling BYTECODE_REDUCE_REF.  This feature is still beta.  Comment out this line if you are sure.
#endif

/******************************************************************************
 * The server can merge duplicate strings on load to conserve memory.  This
 * involves a rather expensive step at startup to dispose of the table used
 * to find the duplicates.  This should be improved eventually, but you may
 * want to trade off faster startup time for increased memory usage.
 *
 * You might want to turn this off if you see a large delay before the
 * INTERN: lines in the log at startup.
 ******************************************************************************
 */

#define STRING_INTERNING	/* */

/******************************************************************************
 * Store the length of the string WITH the string rather than recomputing
 * it each time it is needed.
 ******************************************************************************
 */
/* #define MEMO_STRLEN */


/* #define NETWORK_IDENT */

/*****************************************************************************
 ********** You shouldn't need to change anything below this point. **********
 *****************************************************************************/

#ifndef OUT_OF_BAND_PREFIX
#define OUT_OF_BAND_PREFIX ""
#endif
#ifndef OUT_OF_BAND_QUOTE_PREFIX
#define OUT_OF_BAND_QUOTE_PREFIX ""
#endif

#if PATTERN_CACHE_SIZE < 1
#  error Illegal match() pattern cache size!
#endif

#define NP_SINGLE	1
#define NP_TCP		2
#define NP_LOCAL	3

#define NS_BSD		1
#define NS_SYSV		2

#define MP_SELECT	1
#define MP_POLL		2
#define MP_FAKE		3

#include "config.h"

#ifdef CONFIG_PATTERN_CACHE_SIZE
#  undef PATTERN_CACHE_SIZE
#  define PATTERN_CACHE_SIZE CONFIG_PATTERN_CACHE_SIZE
#endif

#if defined(CONFIG_INPUT_APPLY_BACKSPACE) && !CONFIG_INPUT_APPLY_BACKSPACE
#  undef INPUT_APPLY_BACKSPACE
#endif

#if defined(CONFIG_IGNORE_PROP_PROTECTED) && !CONFIG_IGNORE_PROP_PROTECTED
#  undef IGNORE_PROP_PROTECTED
#endif

#if NETWORK_PROTOCOL != NP_SINGLE  &&  !defined(MPLEX_STYLE)
#  if NETWORK_STYLE == NS_BSD
#    if HAVE_SELECT
#      define MPLEX_STYLE MP_SELECT
#    else
#error You cannot use BSD sockets without having select()!
#    endif
#  else				/* NETWORK_STYLE == NS_SYSV */
#    if NETWORK_PROTOCOL == NP_LOCAL
#      if SELECT_WORKS_ON_FIFOS
#        define MPLEX_STYLE MP_SELECT
#      else
#        if POLL_WORKS_ON_FIFOS
#	   define MPLEX_STYLE MP_POLL
#	 else
#	   if FSTAT_WORKS_ON_FIFOS
#	     define MPLEX_STYLE MP_FAKE
#	   else
#error I need to be able to do a multiplexing wait on FIFOs!
#	   endif
#	 endif
#      endif
#    else			/* It's a TLI-based networking protocol */
#      if HAVE_POLL
#        define MPLEX_STYLE MP_POLL
#      else
#error You cannot use TLI without having poll()!
#      endif
#    endif
#  endif
#endif

#if (NETWORK_PROTOCOL == NP_LOCAL || NETWORK_PROTOCOL == NP_SINGLE) && defined(OUTBOUND_NETWORK)
#  error You cannot define "OUTBOUND_NETWORK" with that "NETWORK_PROTOCOL"
#endif

/* make sure OUTBOUND_NETWORK has a value;
   for backward compatibility, use 1 if none given */
#if defined(OUTBOUND_NETWORK) && (( 0 * OUTBOUND_NETWORK - 1 ) == 0)
#undef OUTBOUND_NETWORK
#define OUTBOUND_NETWORK 1
#endif


#if NETWORK_PROTOCOL != NP_LOCAL && NETWORK_PROTOCOL != NP_SINGLE && NETWORK_PROTOCOL != NP_TCP
#  error Illegal value for "NETWORK_PROTOCOL"
#endif

#if NETWORK_STYLE != NS_BSD && NETWORK_STYLE != NS_SYSV
#  error Illegal value for "NETWORK_STYLE"
#endif

#if defined(MPLEX_STYLE) 	\
    && MPLEX_STYLE != MP_SELECT \
    && MPLEX_STYLE != MP_POLL \
    && MPLEX_STYLE != MP_FAKE
#  error Illegal value for "MPLEX_STYLE"
#endif

#endif				/* !Options_h */
