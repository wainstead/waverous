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

#ifndef Storage_h
#define Storage_h 1

#include "config.h"
#include "structures.h"

typedef enum Memory_Type {
    M_AST_POOL, M_AST, M_PROGRAM, M_PVAL, M_NETWORK, M_STRING, M_VERBDEF,
    M_LIST, M_PREP, M_PROPDEF, M_OBJECT_TABLE, M_OBJECT, M_FLOAT,
    M_STREAM, M_NAMES, M_ENV, M_TASK, M_PATTERN,

    M_BYTECODES, M_FORK_VECTORS, M_LIT_LIST,
    M_PROTOTYPE, M_CODE_GEN, M_DISASSEMBLE, M_DECOMPILE,

    M_RT_STACK, M_RT_ENV, M_BI_FUNC_DATA, M_VM,

    M_REF_ENTRY, M_REF_TABLE,

    Sizeof_Memory_Type

} Memory_Type;

extern char	       *str_dup(const char *);
extern const char      *str_ref(const char *);
extern void        	free_str(const char *);
extern Var		memory_usage(void);

extern void		myfree(void *where, Memory_Type type);
extern void	       *mymalloc(unsigned size, Memory_Type type);

#endif /* Storage_h */

/* $Log: not supported by cvs2svn $
 * Revision 2.1  1996/02/08  06:13:09  pavel
 * Added M_FLOAT, removed unused M_PI.  Updated copyright notice for 1996.
 * Release 1.8.0beta1.
 *
 * Revision 2.0  1995/11/30  04:55:11  pavel
 * New baseline version, corresponding to release 1.8.0alpha1.
 *
 * Revision 1.7  1992/10/23  23:03:47  pavel
 * Added copyright notice.
 *
 * Revision 1.6  1992/10/21  03:02:35  pavel
 * Converted to use new automatic configuration system.
 *
 * Revision 1.5  1992/10/17  20:53:28  pavel
 * Global rename of strdup->str_dup, strref->str_ref, vardup->var_dup, and
 * varref->var_ref, to avoid conflict on systems that provide their own
 * definition of strdup().
 *
 * Revision 1.4  1992/09/14  17:39:30  pjames
 * Moved db_modification code to db modules.
 *
 * Revision 1.3  1992/08/31  22:23:29  pjames
 * Changed some `char *'s to `const char *'
 *
 * Revision 1.2  1992/08/28  16:25:17  pjames
 * Added memory types for refcount module.  Added `strref()' and `free_str()'.
 *
 * Revision 1.1  1992/08/10  16:20:00  pjames
 * Initial RCS-controlled version
 */
