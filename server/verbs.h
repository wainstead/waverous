/******************************************************************************
  Copyright (c) 1995, 1996 Xerox Corporation.  All rights reserved.
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

#include "db.h"
#include "structures.h"

extern enum error	validate_verb_descriptor(Var desc);
extern db_verb_handle	find_described_verb(Objid oid, Var desc);

/* $Log: not supported by cvs2svn $
 * Revision 2.1  1996/02/08  06:07:21  pavel
 * Updated copyright notice for 1996.  Release 1.8.0beta1.
 *
 * Revision 2.0  1995/12/28  00:16:05  pavel
 * Release 1.8.0alpha3.
 *
 * Revision 1.1  1995/12/28  00:14:25  pavel
 * Initial revision
 */
