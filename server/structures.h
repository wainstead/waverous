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

#ifndef Structures_h
#define Structures_h 1

#include "my-stdio.h"

#include "config.h"

typedef int32	Objid;

/*
 * Special Objid's
 */
#define SYSTEM_OBJECT	0
#define NOTHING		-1
#define AMBIGUOUS	-2
#define FAILED_MATCH	-3

/* Do not reorder or otherwise modify this list, except to add new elements at
 * the end, since the order here defines the numeric equivalents of the error
 * values, and those equivalents are both DB-accessible knowledge and stored in
 * raw form in the DB.
 */
enum error {
    E_NONE, E_TYPE, E_DIV, E_PERM, E_PROPNF, E_VERBNF, E_VARNF, E_INVIND,
    E_RECMOVE, E_MAXREC, E_RANGE, E_ARGS, E_NACC, E_INVARG, E_QUOTA, E_FLOAT
};

/* Do not reorder or otherwise modify this list, except to add new elements at
 * the end, since the order here defines the numeric equivalents of the type
 * values, and those equivalents are both DB-accessible knowledge and stored in
 * raw form in the DB.
 */
typedef enum {
    TYPE_INT, TYPE_OBJ, TYPE_STR, TYPE_ERR, TYPE_LIST, /* user-visible */
    TYPE_CLEAR,			/* in clear properties' value slot */
    TYPE_NONE,			/* in uninitialized MOO variables */
    TYPE_CATCH,			/* on-stack marker for an exception handler */
    TYPE_FINALLY,		/* on-stack marker for a TRY-FINALLY clause */
    TYPE_FLOAT			/* floating-point number; user-visible */
} var_type;

#define TYPE_ANY ((var_type) -1) /* wildcard for use in declaring built-ins */
#define TYPE_NUMERIC ((var_type) -2) /* wildcard for (integer or float) */

typedef struct Var  Var;

struct Var {
    union {
	const char     *str;	/* STR */
	int32		num;	/* NUM, CATCH, FINALLY */
	Objid		obj;	/* OBJ */
	enum error	err;	/* ERR */
	Var	       *list;	/* LIST */
	double	       *fnum;	/* FLOAT */
    } v;
    var_type	type;
};

extern Var	zero;		/* useful constant */

#endif /* !Structures_h */

/* $Log: not supported by cvs2svn $
 * Revision 2.1  1996/02/08  06:12:21  pavel
 * Added E_FLOAT, TYPE_FLOAT, and TYPE_NUMERIC.  Renamed TYPE_NUM to TYPE_INT.
 * Updated copyright notice for 1996.  Release 1.8.0beta1.
 *
 * Revision 2.0  1995/11/30  04:55:46  pavel
 * New baseline version, corresponding to release 1.8.0alpha1.
 *
 * Revision 1.12  1992/10/23  23:03:47  pavel
 * Added copyright notice.
 *
 * Revision 1.11  1992/10/21  03:02:35  pavel
 * Converted to use new automatic configuration system.
 *
 * Revision 1.10  1992/09/14  17:40:51  pjames
 * Moved db_modification code to db modules.
 *
 * Revision 1.9  1992/09/04  01:17:29  pavel
 * Added support for the `f' (for `fertile') bit on objects.
 *
 * Revision 1.8  1992/09/03  16:25:12  pjames
 * Added TYPE_CLEAR for Var.
 * Changed Property definition lists to be arrays instead of linked lists.
 *
 * Revision 1.7  1992/08/31  22:25:04  pjames
 * Changed some `char *'s to `const char *'
 *
 * Revision 1.6  1992/08/14  00:00:36  pavel
 * Converted to a typedef of `var_type' = `enum var_type'.
 *
 * Revision 1.5  1992/08/10  16:52:00  pjames
 * Moved several types/procedure declarations to storage.h
 *
 * Revision 1.4  1992/07/30  21:24:31  pjames
 * Added M_COND_ARM_STACK and M_STMT_STACK for vector.c
 *
 * Revision 1.3  1992/07/28  17:18:48  pjames
 * Added M_COND_ARM_STACK for unparse.c
 *
 * Revision 1.2  1992/07/27  18:21:34  pjames
 * Changed name of ct_env to var_names, const_env to literals and
 * f_vectors to fork_vectors, removed M_CT_ENV, M_LOCAL_ENV, and
 * M_LABEL_MAPS, changed M_CONST_ENV to M_LITERALS, M_IM_STACK to
 * M_EXPR_STACK, M_F_VECTORS to M_FORK_VECTORS, M_ID_LIST to M_VL_LIST
 * and M_ID_VALUE to M_VL_VALUE.
 *
 * Revision 1.1  1992/07/20  23:23:12  pavel
 * Initial RCS-controlled version.
 */
