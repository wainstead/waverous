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

#ifndef Sym_Table_h
#define Sym_Table_h 1

#include "config.h"
#include "version.h"

typedef struct {
    unsigned max_size;
    unsigned size;
    const char **names;
} Names;

extern Names *new_builtin_names(DB_Version, int);
extern int first_user_slot(DB_Version, int);
extern unsigned find_or_add_name(Names **, const char *);
extern int find_name(Names *, const char *);
extern void free_names(Names *);
extern Names *copy_names(Names *);

/* Old environment slots for built-in variables */
#define OLD_SLOT_NUM	0
#define OLD_SLOT_OBJ	1
#define OLD_SLOT_STR	2
#define OLD_SLOT_LIST	3
#define OLD_SLOT_ERR	4
#define OLD_SLOT_PLAYER	5
#define OLD_SLOT_THIS	6
#define OLD_SLOT_CALLER	7
#define OLD_SLOT_VERB	8
#define OLD_SLOT_ARGS	9
#define OLD_SLOT_ARGSTR	10
#define OLD_SLOT_DOBJ	11
#define OLD_SLOT_DOBJSTR	12
#define OLD_SLOT_PREPSTR	13
#define OLD_SLOT_IOBJ	14
#define OLD_SLOT_IOBJSTR	15

/* Added in DBV_Float: */
#define OLD_SLOT_INT	16
#define OLD_SLOT_FLOAT	17

/* Current slots as of DBV_Constants */
#define SLOT_PLAYER	0
#define SLOT_THIS	1
#define SLOT_CALLER	2
#define SLOT_VERB	3
#define SLOT_ARGS	4
#define SLOT_ARGSTR	5
#define SLOT_DOBJ	6
#define SLOT_DOBJSTR	7
#define SLOT_PREPSTR	8
#define SLOT_IOBJ	9
#define SLOT_IOBJSTR	10

#endif				/* !Sym_Table_h */

/* 
 * $Log: not supported by cvs2svn $
 * Revision 1.3.6.1  2002/09/12 05:57:40  xplat
 * Changes for inline PC saving and patch tags in the on-disk DB.
 *
 * Revision 1.3  1998/12/14 13:19:06  nop
 * Merge UNSAFE_OPTS (ref fixups); fix Log tag placement to fit CVS whims
 *
 * Revision 1.2  1997/03/03 04:19:30  nop
 * GNU Indent normalization
 *
 * Revision 1.1.1.1  1997/03/03 03:45:04  nop
 * LambdaMOO 1.8.0p5
 *
 * Revision 2.2  1996/03/10  01:15:53  pavel
 * Removed a number of obsolete declarations.  Release 1.8.0.
 *
 * Revision 2.1  1996/02/08  06:11:23  pavel
 * Made new_builtin_names() and first_user_slot() version-dependent to support
 * version numbers on suspended-task frames.  Added SLOT_INT and SLOT_FLOAT.
 * Updated copyright notice for 1996.  Release 1.8.0beta1.
 *
 * Revision 2.0  1995/11/30  04:55:57  pavel
 * New baseline version, corresponding to release 1.8.0alpha1.
 *
 * Revision 1.4  1992/10/23  23:03:47  pavel
 * Added copyright notice.
 *
 * Revision 1.3  1992/10/21  03:02:35  pavel
 * Converted to use new automatic configuration system.
 *
 * Revision 1.2  1992/08/31  22:24:26  pjames
 * Changed some `char *'s to `const char *'
 *
 * Revision 1.1  1992/07/20  23:23:12  pavel
 * Initial RCS-controlled version.
 */
