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

#include "config.h"
#include "storage.h"
#include "structures.h"
#include "rt_const.h"
#include "utils.h"

Var rt_const_values[SizeOf_Constant_Slot];
const char *rt_const_names[SizeOf_Constant_Slot] = {
	"NUM",
	"OBJ",
	"STR",
	"LIST",
	"ERR",
	"INT",
	"FLOAT",
};

void
initialize_rt_consts(void)
{
    Var v;

    v.type = TYPE_INT;
    v.v.num = (int) TYPE_ERR;
    rt_const_values[CSLOT_ERR] = var_ref(v);
    v.v.num = (int) TYPE_INT;
    rt_const_values[CSLOT_NUM] = var_ref(v);
    v.v.num = (int) _TYPE_STR;
    rt_const_values[CSLOT_STR] = var_ref(v);
    v.v.num = (int) TYPE_OBJ;
    rt_const_values[CSLOT_OBJ] = var_ref(v);
    v.v.num = (int) _TYPE_LIST;
    rt_const_values[CSLOT_LIST] = var_ref(v);
    v.v.num = (int) TYPE_INT;
    rt_const_values[CSLOT_INT] = var_ref(v);
    v.v.num = (int) _TYPE_FLOAT;
    rt_const_values[CSLOT_FLOAT] = var_ref(v);
}

char rcsid_rt_const[] = "$Id: rt_const.c,v 1.1.2.1 2002-11-03 03:37:58 xplat Exp $";


/* 
 * $Log: not supported by cvs2svn $
 */
