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

#ifndef Version_H
#define Version_H 1

#include "config.h"

extern const char *server_version;

/* The following list must never be reordered, only appended to.  There is one
 * element per version of the database format (including incompatible changes
 * to the language, such as the addition of new keywords).  The integer value
 * of each element is used in the DB header on disk to identify the format
 * version in use in that file.
 */
typedef enum {
    DBV_Prehistory,		/* Before format versions */
    DBV_Exceptions,		/* Addition of the `try', `except', `finally',
				 * and `endtry' keywords.
				 */
    DBV_BreakCont,		/* Addition of the `break' and `continue'
				 * keywords.
				 */
    DBV_Float,			/* Addition of `FLOAT' and `INT' variables and
				 * the `E_FLOAT' keyword, along with version
				 * numbers on each frame of a suspended task.
				 */
    DBV_BFBugFixed,		/* Bug in built-in function overrides fixed by
				 * making it use tail-calling.  This DB_Version
				 * change exists solely to turn off special
				 * bug handling in read_bi_func_data().
				 */
    Num_DB_Versions		/* Special: the current version is this - 1. */
} DB_Version;

#define current_version	((DB_Version) (Num_DB_Versions - 1))

extern int check_version(DB_Version);
				/* Returns true iff given version is within the
				 * known range.
				 */

#endif				/* !Version_H */
