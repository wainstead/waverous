/* C code produced by gperf version 2.7.2 */
/* Hacked by Xplat after an old patch by Pavel Curtis */
/* Command-line: ./igperf -aCxptT -k'1,3,$' keywords.gperf  */	/* -*- C -*- */

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

#include "my-string.h"
#include "my-ctype.h"

#include "config.h"
#include "keywords.h"
#include "tokens.h"
#include "utils.h"


#define TOTAL_KEYWORDS 42
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 103
/* maximum key range = 101, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104,  30,   0,  50,   0,   0,
       27,   5, 104,  30,   5,  50,   0,  10,   5,   0,
       40,   5,   0,  15,  20, 104,  45,   5, 104,  35,
      104, 104, 104, 104, 104, 104, 104,  30,   0,  50,
        0,   0,  27,   5, 104,  30,   5,  50,   0,  10,
        5,   0,  40,   5,   0,  15,  20, 104,  45,   5,
      104,  35, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104, 104, 104, 104, 104,
      104, 104, 104, 104, 104, 104
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static int
case_strcmp (str, key)
     register char *str, *key;
{
  int ans = 0;

  while (!(ans = tolower(*str) - (int) *key) && *str)
    str++, key++;

  return ans;
}

#ifdef __GNUC__
__inline
#endif
const struct keyword *
in_word_set (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct keyword wordlist[] =
    {
      {""}, {""}, {""},
      {"err",		DBV_Prehistory, tCONSTANT,	E_NONE,	CSLOT_ERR},
      {""}, {""},
      {"endfor",		DBV_Prehistory, tENDFOR},
      {"e_range",	DBV_Prehistory, tERROR,	E_RANGE},
      {"endwhile",	DBV_Prehistory, tENDWHILE},
      {"e_recmove",	DBV_Prehistory, tERROR,	E_RECMOVE},
      {""},
      {"e_none",		DBV_Prehistory, tERROR,	E_NONE},
      {""},
      {"obj",		DBV_Prehistory, tCONSTANT,	E_NONE,	CSLOT_OBJ},
      {""}, {""}, {""}, {""},
      {"str",		DBV_Prehistory,	tCONSTANT,	E_NONE,	CSLOT_STR},
      {"else",		DBV_Prehistory, tELSE},
      {""}, {""}, {""}, {""}, {""}, {""},
      {"e_type",		DBV_Prehistory, tERROR,	E_TYPE},
      {""},
      {"num",		DBV_Prehistory,	tCONSTANT,	E_NONE,	CSLOT_NUM},
      {""},
      {"for",		DBV_Prehistory, tFOR},
      {"return",		DBV_Prehistory, tRETURN},
      {"endif",		DBV_Prehistory, tENDIF},
      {""}, {""}, {""}, {""},
      {"in",		DBV_Prehistory, tIN},
      {"e_invind",	DBV_Prehistory, tERROR,	E_INVIND},
      {"list",		DBV_Prehistory, tCONSTANT,	E_NONE,	CSLOT_LIST},
      {"while",		DBV_Prehistory, tWHILE},
      {"endtry",		DBV_Exceptions, tENDTRY},
      {"e_quota",	DBV_Prehistory, tERROR,	E_QUOTA},
      {"e_invarg",	DBV_Prehistory, tERROR,	E_INVARG},
      {""}, {""}, {""}, {""},
      {"elseif",		DBV_Prehistory, tELSEIF},
      {""},
      {"e_div",		DBV_Prehistory, tERROR,	E_DIV},
      {"e_args",		DBV_Prehistory, tERROR,	E_ARGS},
      {"float",		DBV_Float,	tCONSTANT,	E_NONE,	CSLOT_FLOAT},
      {""},
      {"e_float",	DBV_Float,	tERROR,	E_FLOAT},
      {"break",		DBV_BreakCont, tBREAK},
      {"e_perm",		DBV_Prehistory, tERROR,	E_PERM},
      {"endfork",	DBV_Prehistory, tENDFORK},
      {""},
      {"if",		DBV_Prehistory, tIF},
      {""},
      {"e_nacc",		DBV_Prehistory, tERROR,	E_NACC},
      {""},
      {"continue",	DBV_BreakCont, tCONTINUE},
      {""}, {""}, {""}, {""},
      {"e_maxrec",	DBV_Prehistory, tERROR,	E_MAXREC},
      {""}, {""}, {""}, {""},
      {"int",		DBV_Float,	tCONSTANT,	E_NONE,	CSLOT_INT},
      {"finally",	DBV_Exceptions, tFINALLY},
      {"e_propnf",	DBV_Prehistory, tERROR,	E_PROPNF},
      {"except",		DBV_Exceptions, tEXCEPT},
      {""}, {""},
      {"e_varnf",	DBV_Prehistory, tERROR,	E_VARNF},
      {"e_verbnf",	DBV_Prehistory, tERROR,	E_VERBNF},
      {"fork",		DBV_Prehistory, tFORK},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
      {"try",		DBV_Exceptions, tTRY},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"any",		DBV_Exceptions, tANY}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (tolower(*str) == *s && !case_strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

const struct keyword *
find_keyword(const char *word)
{
    return in_word_set(word, strlen(word));
}

char rcsid_keywords[] = "$Id: keywords.c,v 1.3.8.1 2002-11-03 03:37:58 xplat Exp $";

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  1997/03/03 03:45:02  nop
 * LambdaMOO 1.8.0p5
 *
 * Revision 2.2  1996/02/08  06:33:21  pavel
 * Added `break', `continue', and E_FLOAT.  Updated copyright notice for 1996.
 * Release 1.8.0beta1.
 *
 * Revision 2.1  1995/12/11  08:15:42  pavel
 * Added #include "tokens.h" removed from keywords.h.  Release 1.8.0alpha2.
 *
 * Revision 2.0  1995/11/30  05:02:56  pavel
 * New baseline version, corresponding to release 1.8.0alpha1.
 *
 * Revision 1.1  1995/11/30  05:01:47  pavel
 * Initial revision
 */
