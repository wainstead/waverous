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

#include "options.h"

#ifdef USE_GNU_MALLOC

#include "exceptions.h"

#define MSTATS
#define rcheck
#define botch panic

#include "gnu-malloc.c"

unsigned
malloc_real_size(void *ptr)
{
    char *cp = (char *) ptr;
    struct mhead *p = (struct mhead *) (cp - ((sizeof *p + 7) & ~7));

    return 1 << (p->mh_index + 3);
}

unsigned
malloc_size(void *ptr)
{
    char *cp = (char *) ptr;
    struct mhead *p = (struct mhead *) (cp - ((sizeof *p + 7) & ~7));

#ifdef rcheck
    return p->mh_nbytes;
#else
    return p->mh_index >= 13 ? (1 << (p->mh_index + 3)) : p->mh_size;
#endif
}

#else				/* !defined(USE_GNU_MALLOC) */

int malloc_dummy;		/* Prevent `empty compilation unit' warning */

#endif

