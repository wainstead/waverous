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

#ifndef RT_CONST_h
#define RT_CONST_h 1

#include "config.h"
#include "structures.h"

enum Constant_Slot {
    CSLOT_NUM,
    CSLOT_OBJ,
    CSLOT_STR,
    CSLOT_LIST,
    CSLOT_ERR,
    CSLOT_INT,
    CSLOT_FLOAT,
    SizeOf_Constant_Slot,
};

extern Var rt_const_values[SizeOf_Constant_Slot];
extern const char *rt_const_names[SizeOf_Constant_Slot];

extern void initialize_rt_consts(void);

#endif				/* !RT_CONST_h */

/* 
 * $Log: not supported by cvs2svn $
 */
