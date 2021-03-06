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

typedef int32 Objid;

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
    E_RECMOVE, E_MAXREC, E_RANGE, E_ARGS, E_NACC, E_INVARG, E_QUOTA,
	E_FLOAT
};

/* Do not reorder or otherwise modify this list, except to add new elements at
 * the end, since the order here defines the numeric equivalents of the type
 * values, and those equivalents are both DB-accessible knowledge and stored in
 * raw form in the DB.
 * (note: replacing the enum here with ints, to satisfy g++'s loathing of varargs).
 */
//typedef enum {
//    TYPE_INT, TYPE_OBJ, _TYPE_STR, TYPE_ERR, _TYPE_LIST, /* user-visible */
//    TYPE_CLEAR,                       /* in clear properties' value slot */
//    TYPE_NONE,                        /* in uninitialized MOO variables */
//    TYPE_CATCH,                       /* on-stack marker for an exception handler */
//    TYPE_FINALLY,             /* on-stack marker for a TRY-FINALLY clause */
//    _TYPE_FLOAT                       /* floating-point number; user-visible */
//} var_type;

// the following replace the above typdef enum var_type
typedef int var_type;
const var_type TYPE_INT = 0;
const var_type TYPE_OBJ = 1;
const var_type _TYPE_STR = 2;
const var_type TYPE_ERR = 3;
const var_type _TYPE_LIST = 4;
const var_type TYPE_CLEAR = 5;
const var_type TYPE_NONE = 6;
const var_type TYPE_CATCH = 7;
const var_type TYPE_FINALLY = 8;
const var_type _TYPE_FLOAT = 9;

/* Types which have external data should be marked with the TYPE_COMPLEX_FLAG
 * so that free_var/var_ref/var_dup can recognize them easily.  This flag is
 * only set in memory.  The original _TYPE values are used in the database
 * file and returned to verbs calling typeof().  This allows the inlines to
 * be extremely cheap (both in space and time) for simple types like oids
 * and ints.
 */
#define TYPE_DB_MASK		0x7f
#define TYPE_COMPLEX_FLAG	0x80

#define TYPE_STR		(_TYPE_STR | TYPE_COMPLEX_FLAG)
#define TYPE_FLOAT		(_TYPE_FLOAT | TYPE_COMPLEX_FLAG)
#define TYPE_LIST		(_TYPE_LIST | TYPE_COMPLEX_FLAG)

#define TYPE_ANY ((var_type) -1)	/* wildcard for use in declaring built-ins */
#define TYPE_NUMERIC ((var_type) -2)	/* wildcard for (integer or float) */

typedef struct Var Var;

/* Experimental.  On the Alpha, DEC cc allows us to specify certain
 * pointers to be 32 bits, but only if we compile and link with "-taso
 * -xtaso" in CFLAGS, which limits us to a 31-bit address space.  This
 * could be a win if your server is thrashing.  Running JHM's db, SIZE
 * went from 50M to 42M.  No doubt these pragmas could be applied
 * elsewhere as well, but I know this at least manages to load and run
 * a non-trivial db.
 */

/* #define SHORT_ALPHA_VAR_POINTERS 1 */

#ifdef SHORT_ALPHA_VAR_POINTERS
#pragma pointer_size save
#pragma pointer_size short
#endif

struct Var {
    union {
	const char *str;	/* STR */
	int32 num;		/* NUM, CATCH, FINALLY */
	Objid obj;		/* OBJ */
	enum error err;		/* ERR */
	Var *list;		/* LIST */
	double *fnum;		/* FLOAT */
    } v;
    var_type type;
};

#ifdef SHORT_ALPHA_VAR_POINTERS
#pragma pointer_size restore
#endif

extern Var zero;		/* useful constant */

#endif				/* !Structures_h */

