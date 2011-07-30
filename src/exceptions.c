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

/* Copyright 1989 Digital Equipment Corporation.                             */
/* Distributed only by permission.                                           */
/*****************************************************************************/
/* File: exceptions.c                                                        */
/* Taken originally from:                                                    */
/*              Implementing Exceptions in C                                 */
/*              Eric S. Roberts                                              */
/*              Research Report #40                                          */
/*              DEC Systems Research Center                                  */
/*              March 21, 1989                                               */
/* Modified slightly by Pavel Curtis for use in the LambdaMOO server code.   */
/* ------------------------------------------------------------------------- */
/* Implementation of the C exception handler.  Most of the real work is      */
/* done in the macros defined in the exceptions.h header file.               */
/*****************************************************************************/

#include <setjmp.h>

#include "exceptions.h"

ES_CtxBlock *ES_exceptionStack = 0;

Exception ANY;

void
ES_RaiseException(Exception * exception, int value)
{
    ES_CtxBlock *cb, *xb;
    int i;

    for (xb = ES_exceptionStack; xb; xb = xb->link) {
	for (i = 0; i < xb->nx; i++) {
	    if (xb->array[i] == exception || xb->array[i] == &ANY)
		goto doneSearching;
	}
    }

  doneSearching:
    if (!xb)
	panic("Unhandled exception!");

    for (cb = ES_exceptionStack; cb != xb && !cb->_finally; cb = cb->link);
    ES_exceptionStack = cb;
    cb->id = exception;
    cb->value = value;
    longjmp(cb->jmp, 1);
}

