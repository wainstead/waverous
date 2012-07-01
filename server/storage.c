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

#include "my-stdlib.h"

#include "config.h"
#include "exceptions.h"
#include "list.h"
#include "options.h"
#include "storage.h"
#include "structures.h"
#include "utils.h"

static unsigned alloc_num[Sizeof_Memory_Type];


/**
 * explanation of static inline functions:
 * "If you wanted to keep your inline function entirely private to one
 * translation unit, you make it static inline."
 *
 * C's 'inline' does pretty much what C++'s does.
 */

static inline int
refcount_overhead(Memory_Type type)
{
    /* These are the only allocation types that are addref()'d.
     * As long as we're living on the wild side, avoid getting the
     * refcount slot for allocations that won't need it.
     */
    switch (type) {
    case M_FLOAT:
	/* for systems with picky double alignment */
	return MAX(sizeof(int), sizeof(double));
    case M_STRING:
#ifdef MEMO_STRLEN
	return sizeof(int) + sizeof(int);
#else
	return sizeof(int);
#endif				/* MEMO_STRLEN */
    case M_LIST:
	/* for systems with picky pointer alignment */
	return MAX(sizeof(int), sizeof(Var *));
    default:
	return 0;
    }
}

void *
mymalloc(unsigned size, Memory_Type type)
{
    char *memptr;
    char msg[100];
    int offs;

    if (size == 0)		/* For queasy systems */
	size = 1;

    offs = refcount_overhead(type);
    memptr = (char *) malloc(size + offs);
    if (!memptr) {
	sprintf(msg, "memory allocation (size %u) failed!", size);
	panic(msg);
    }
    alloc_num[type]++;

    if (offs) {
	memptr += offs;
	((int *) memptr)[-1] = 1;
#ifdef MEMO_STRLEN
	if (type == M_STRING)
	    ((int *) memptr)[-2] = size - 1;
#endif				/* MEMO_STRLEN */
    }
    return memptr;
}

const char *
str_ref(const char *s)
{
    addref(s);
    return s;
}

char *
str_dup(const char *s)
{
    char *r;

    if (s == 0 || *s == '\0') {
	static char *emptystring;

	if (!emptystring) {
	    emptystring = (char *) mymalloc(1, M_STRING);
	    *emptystring = '\0';
	}
	addref(emptystring);
	return emptystring;
    } else {
	r = (char *) mymalloc(strlen(s) + 1, M_STRING);	/* NO MEMO HERE */
	strcpy(r, s);
    }
    return r;
}

void *
myrealloc(void *ptr, unsigned size, Memory_Type type)
{
    int offs = refcount_overhead(type);
    static char msg[100];


	ptr = realloc((char *) ptr - offs, size + offs);
	if (!ptr) {
	    sprintf(msg, "memory re-allocation (size %u) failed!", size);
	    panic(msg);
	}

    return (char *) ptr + offs;
}

void
myfree(void *ptr, Memory_Type type)
{
    alloc_num[type]--;

    free((char *) ptr - refcount_overhead(type));
}


/* XXX stupid fix for non-gcc compilers, already in storage.h */
#ifdef NEVER
void
free_str(const char *s)
{
    if (delref(s) == 0)
	myfree((void *) s, M_STRING);
}

#endif

Var
memory_usage(void)
{
    Var r;
    r = new_list(0);
    return r;
}

