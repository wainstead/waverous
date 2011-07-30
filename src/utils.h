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

#ifndef Utils_h
#define Utils_h 1

#include "my-stdio.h"

#include "config.h"
#include "execute.h"

#undef MAX
#undef MIN
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define Arraysize(x) (sizeof(x) / sizeof(*x))

extern int mystrcasecmp(const char *, const char *);
extern int mystrncasecmp(const char *, const char *, int);

extern int verbcasecmp(const char *verb, const char *word);

extern unsigned str_hash(const char *);

extern void complex_free_var(Var);
extern Var complex_var_ref(Var);
extern Var complex_var_dup(Var);
extern int var_refcount(Var);

static inline void
free_var(Var v)
{
    if (v.type & TYPE_COMPLEX_FLAG)
	complex_free_var(v);
}

static inline Var
var_ref(Var v)
{
    if (v.type & TYPE_COMPLEX_FLAG)
	return complex_var_ref(v);
    else
	return v;
}

static inline Var
var_dup(Var v)
{
    if (v.type & TYPE_COMPLEX_FLAG)
	return complex_var_dup(v);
    else
	return v;
}

extern int equality(Var lhs, Var rhs, int case_matters);
extern int is_true(Var v);

extern char *strsub(const char *, const char *, const char *, int);
extern int strindex(const char *, const char *, int);
extern int strrindex(const char *, const char *, int);

extern Var get_system_property(const char *);
extern Objid get_system_object(const char *);

extern int value_bytes(Var);

extern const char *raw_bytes_to_binary(const char *buffer, int buflen);
extern const char *binary_to_raw_bytes(const char *binary, int *rawlen);

#endif

