/***************************************************************************
 * Moo Database Browser 1.2                                                *
 * Command-line extraction of verbs/properties/etc from Lambda Moo DBs.    *
 * Copyright (C) 2005 Neil Fraser                                          *
 * http://neil.fraser.name/software/moobrowser/                            *
 *                                                                         *
 * This program is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU General Public License             *
 * as published by the Free Software Foundation.                           *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License (www.gnu.org) for more details.              *
 *                                                                         *
 * Compile with:  gcc moobrowser.c -o moobrowser                           *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *databasename;
char tmp_int[20];

/* The mode determines what data we're looking for in the database. */
int mode = 0;
#define SUMMARY 0
#define OBJECTS 1
#define PLAYERS 2
#define CLOCKS 3
#define QUEUEDTASKS 4
#define SUSPENDEDTASKS 5
#define CONNECTIONS 6
#define OBJECT 10
#define VERB 11
#define PROP 12
int mode_object = -1;
int mode_verb = -1;
char mode_prop[512];
char mode_verbstr[42];

/* Flag constants */
#define OB_USER       01
#define OB_PROGRAMMER 02
#define OB_WIZARD     04
#define OB_OBSOLETE_1 010
#define OB_READ       020
#define OB_WRITE      040
#define OB_OBSOLETE_2 0100
#define OB_FERTILE    0200
#define PF_READ  01
#define PF_WRITE 02
#define PF_CHOWN 04
#define VF_READ  01
#define VF_WRITE 02
#define VF_EXEC  04
#define VF_DEBUG 010
#define VF_NOT_O 02000

/* Verb argument constants */
/* To update, copy this from db_verbs.c */
static const char *prep_list[] =
{
    "with/using",
    "at/to",
    "in front of",
    "in/inside/into",
    "on top of/on/onto/upon",
    "out of/from inside/from",
    "over",
    "through",
    "under/underneath/beneath",
    "behind",
    "beside",
    "for/about",
    "is",
    "as",
    "off/off of",
};
#define ASPEC_NONE 0
#define ASPEC_ANY  1
#define ASPEC_THIS 2
#define PREP_ANY  -2
#define PREP_NONE -1
#define DOBJSHIFT 4
#define IOBJSHIFT 6
#define OBJMASK 0x3

/* Value type constants */
#define TYPE_INT 0
#define TYPE_OBJ 1
#define TYPE_STR 2
#define TYPE_ERR 3
#define TYPE_LIST 4
#define TYPE_CLEAR 5
#define TYPE_FLOAT 9

/* Here be errors */
static const char *error_list[] =
{
    "E_NONE",
    "E_TYPE",
    "E_DIV",
    "E_PERM",
    "E_PROPNF",
    "E_VERBNF",
    "E_VARNF",
    "E_INVIND",
    "E_RECMOVE",
    "E_MAXREC",
    "E_RANGE",
    "E_ARGS",
    "E_NACC",
    "E_INVARG",
    "E_QUOTA",
    "E_FLOAT",
};

/* A structure defining the attributes of a property. */
struct property {
  int rootobj;
  char *name;
  char *value;
  int clear;
  int owner;
  int perms;
};


/* Read one line from the database, parse it as an integer and return it. */
/* Can't use fscanf because it strips delimiters (in this case eating blank lines). */
#define read_int(number, fh) fgets(tmp_int, 20, fh); number = atoi(tmp_int)

/* Read one line from the database, parse it as an object number and return the number. */
/* '#5' -> 5  '#5 recycled' -> -5 */
int read_obj(FILE *fh) {
  int number;
  char tmp_str[30];
  fgets(tmp_str, 30, fh);
  tmp_str[0] = ' ';
  number = atoi(tmp_str);
  if (strstr(tmp_str, "recycled") != NULL)
    number = -number;
  return number;
}

/* Read one line from the database and return it.
Does not include the trailing \n character. */
char *read_big_str(FILE *fh) {
  static char bufferstr[1024];
  char *lineptr;
  char *oldlineptr;
  char *lfptr;
  int size;

  lineptr = NULL;
  oldlineptr = NULL;
  size = 1;
  do {
    fgets(bufferstr, sizeof(bufferstr), fh);
    if (lineptr)
      oldlineptr = lineptr;
    size += strlen(bufferstr);
    lineptr = (char *) malloc(sizeof(char) * size);
    if (!lineptr) {
      printf("Unable to allocate %u bytes of memory. (read_big_str: lineptr)\n", sizeof(char) * size);
      exit(1);
    }
    if (oldlineptr) {
      strcpy(lineptr, oldlineptr);
      strcat(lineptr, bufferstr);
      free(oldlineptr);
    } else {
      strcpy(lineptr, bufferstr);
    }
  } while (!strrchr(bufferstr, '\n'));

  /* Strip the \n off the line. */
  if ((lfptr = (char *) strchr(lineptr, '\n')))
    *lfptr = '\0';
  return lineptr;
}


/* Read one line from the database.  Throw it out. */
void read_junk_str(FILE *fh) {
  static char junk[1024];

  do
    fgets(junk, sizeof(junk), fh);
  while (!strrchr(junk, '\n'));
}


/* Return the smallest non-null integer of two integers. */
int minpos(int a, int b) {
  if (a > 0 && b <= 0)
    return a;
  if (b > 0 && a <= 0)
    return b;
  if (a > 0 && b > 0)
    return (a > b) ? b : a;
  return 0;
}


/* Open file for input. */
FILE *openfile(char *filename) {
  FILE *fh;
  fh = fopen(filename, "r");
  if (fh == NULL) {
    printf("Unable to open database file: '%s'\n", filename);
    exit(1);
  }
  return fh;
}


/* Parse the intro section */
void parse_intro(char **lineptr, int *nobjs, int *nprogs, int *dummy, int *nusers, FILE *fh) {
  /* First line: "** LambdaMOO Database, Format Version 4 **" */
  *lineptr = read_big_str(fh);

  /* Next four lines */
  read_int(*nobjs, fh);
  read_int(*nprogs, fh);
  read_int(*dummy, fh);
  read_int(*nusers, fh);
}


/* Read in a value and format it.  Set 'size' to be the length of the return string,
or to zero if one doesn't care about the value. */
char *parse_value(int wanted, FILE *fh) {
  char *returnstr;
  char *elementstr;
  char *tmpstr;
  int type, listlength, errornum, first, escapes;

  read_int(type, fh);

  if (!wanted && (type==TYPE_INT || type==TYPE_OBJ || type==TYPE_STR || type==TYPE_ERR || type==TYPE_FLOAT)) {
    /* We don't care about the answer, just eat the next line and move on. */
    read_junk_str(fh);
    return NULL;
  } else if (!wanted && (type==TYPE_CLEAR)) {
    /* There is no next line. */
    return NULL;
  }

  switch (type) {
    case (int) TYPE_INT:
      return read_big_str(fh);
    case TYPE_OBJ:
      returnstr = (char *) malloc(sizeof(char) * 20);
      if (!returnstr) {
        printf("Unable to allocate %u bytes of memory (parse_value: OBJ).\n", sizeof(char) * 20);
        exit(1);
      }
      strcpy(returnstr, "#");
      elementstr = read_big_str(fh);
      strcat(returnstr, elementstr);
      free(elementstr);
      return returnstr;
    case TYPE_STR:
      returnstr = read_big_str(fh);
      /* Count the number of escapes needed. */
      escapes = 0;
      elementstr = returnstr;
      while ((elementstr = (char *) minpos((int) strchr(elementstr, '"'), (int) strchr(elementstr, '\\')))) {
        elementstr++;
        escapes++;
      }
      elementstr = NULL;
      tmpstr = returnstr;
      /* Increase the size of the string to acommodate the two quotes and any escapes. */
      returnstr = (char *) malloc(sizeof(char) * (1+strlen(tmpstr)+escapes+1+1));
      if (!returnstr) {
        printf("Unable to allocate %u bytes of memory (parse_value: STR: main).\n", sizeof(char) * (1+strlen(tmpstr)+escapes+1+1));
        exit(1);
      }
      /* Add a placeholder for the start quote. */
      strcpy(returnstr, " ");
      /* Paste in the unescaped string. */
      strcat(returnstr, tmpstr);
      free(tmpstr);
      tmpstr = NULL;

      /* Escape all " and / characters. */
      elementstr = returnstr;
      while ((elementstr = (char *) minpos((int) strchr(elementstr, '"'), (int) strchr(elementstr, '\\')))) {
        /* Copy the tail of the string into a temporary var. */
        tmpstr = (char *) malloc(sizeof(char) * (strlen(elementstr)+1));
        if (!tmpstr) {
          printf("Unable to allocate %u bytes of memory (parse_value: STR: tail).\n", sizeof(char) * (strlen(elementstr)+1));
          exit(1);
        }
        strcpy(tmpstr, elementstr);
        /* Add the backslash. */
        elementstr[0] = '\\';
        /* Place the tail back onto the string. */
        strcpy(++elementstr, tmpstr);
        free(tmpstr);
        elementstr++;
      }

      /* Add the start and end quotes. */
      returnstr[0] = '"';
      strcat(returnstr, "\"");
      return returnstr;
    case TYPE_ERR:
      returnstr = (char *) malloc(sizeof(char) * 15);
      if (!returnstr) {
        printf("Unable to allocate %u bytes of memory (parse_value: ERR).\n", sizeof(char) * 15);
        exit(1);
      }
      read_int(errornum, fh);
      if (0 <= errornum || errornum < sizeof(error_list) / sizeof(*error_list))
        strcpy(returnstr, error_list[errornum]);
      else {
        printf("Unknown error code: %d\n", errornum); exit(1);
      }
      return returnstr;
    case TYPE_LIST:
      read_int(listlength, fh);
      if (wanted) {
        /* Start the list with a '{' character. */
        /* Allow room for a '}' character and a null character. */
        returnstr = (char *) malloc(sizeof(char) * 3);
        if (!returnstr) {
          printf("Unable to allocate %u bytes of memory (parse_value: LIST: open).\n", sizeof(char) * 3);
          exit(1);
        }
        strcpy(returnstr, "{");
      }
      first = 1;
      for (; listlength > 0; listlength--) {
	elementstr = parse_value(wanted, fh);
        if (wanted) {
          /* Append this element to the list. */
          tmpstr = returnstr;
          /* Allow room for a ', ' string, a '}' character and a null character. */
          returnstr = (char *) malloc(sizeof(char) * (strlen(tmpstr)+2+strlen(elementstr)+1+1));
          if (!returnstr) {
            printf("Unable to allocate %u bytes of memory (parse_value: LIST: element).\n", sizeof(char) * (strlen(tmpstr)+2+strlen(elementstr)+1+1));
            exit(1);
          }
          strcpy(returnstr, tmpstr);
          if (!first) {
            /* Add a comma and space between each element. */
            strcat(returnstr, ", ");
          }
          strcat(returnstr, elementstr);
          free(elementstr);
          free(tmpstr);
        }
        first = 0;
      }
      if (wanted) {
        /* Add the } to the end of the list. */
        /* There will always be space for an extra character. */
        strcat(returnstr, "}");
        return returnstr;
      } else {
        return NULL;
      }
    case TYPE_CLEAR:
      returnstr = (char *) malloc(sizeof(char) * 10);
      if (!returnstr) {
        printf("Unable to allocate %u bytes of memory (parse_value: clear).\n", sizeof(char) * 10);
        exit(1);
      }
      strcpy(returnstr, "[clear]");
      return returnstr;
    case TYPE_FLOAT:
      return read_big_str(fh);
    default:
      printf("Unknown value type: %d\n", type);
      printf("(Non-standard data types such as WAIFs are not supported.)\n");
      exit(1);
  }
  return NULL;
}


/* Reopen the database and look for the properties defined on a specific object. */
/* Returns the object number of the requested object's parent. */
int dig_props(int object, struct property *propstructs, int *propindex) {
  FILE *fh;
  int nobjs, nusers, nprogs, dummy, i, j;
  char *lineptr;

  int objnumber, parent;
  int verbdefs, propdefs, nprops;
  char *pnameptr;
  char *pvalueptr;
  int startindex;

  /* printf("Deep scan for #%d\n", object); */
  /* Open file for input */
  fh = openfile(databasename);

  /* Parse the intro section */
  parse_intro(&lineptr, &nobjs, &nprogs, &dummy, &nusers, fh);
  free(lineptr);

  /* List of players */
  for (i = 1; i <= nusers; i++)
    read_int(dummy, fh);

  /* List of objects */
  parent = -1;
  for (i = 1; i <= nobjs; i++) {
    /* Parse all the lines devoted to the next object in the database. */
    objnumber = read_obj(fh);
    if (objnumber < 0) /* Recycled object */
      continue;
    read_junk_str(fh); /* Name */
    read_junk_str(fh); /* Junk */
    read_int(dummy, fh); /* Flags */
    read_int(dummy, fh); /* Owner */
    read_int(dummy, fh); /* Location */
    read_int(dummy, fh); /* Contents */
    read_int(dummy, fh); /* next */
    read_int(dummy, fh); /* Parent */
    if (objnumber == object)
      parent = dummy;
    read_int(dummy, fh); /* Child */
    read_int(dummy, fh); /* Sibling */
    read_int(verbdefs, fh);
    for (j = 1; j <= verbdefs; j++) {
      /* Parse all the lines devoted to a verb definition */
      read_junk_str(fh);
      read_int(dummy, fh); /* Verb owner */
      read_int(dummy, fh); /* Verb perms */
      read_int(dummy, fh); /* Verb prep */
    }

    startindex = *propindex;
    read_int(propdefs, fh);
    for (j = 1; j <= propdefs; j++) {
      pnameptr = read_big_str(fh);
      if (objnumber == object) {
        *propindex = *propindex + 1;
        propstructs[*propindex].name = pnameptr;
        propstructs[*propindex].rootobj = objnumber;
      } else {
        free(pnameptr);
      }
    }

    read_int(nprops, fh);
    for (j = 1; j <= nprops; j++) {
      pvalueptr = parse_value((objnumber == object), fh);
      /* If the property on the child object is clear, use the parent's value. */
      if (objnumber == object && strcmp(propstructs[startindex+j].value, "[clear]") == 0) {
        free(propstructs[startindex+j].value);
        propstructs[startindex+j].value = pvalueptr;
      } else {
        free(pvalueptr);
      }
      read_int(dummy, fh); /* Prop owner */
      read_int(dummy, fh); /* Prop perms */
    }

    /* We found the target object; bail. */
    if (objnumber == object)
      break;
  }
  fclose(fh);
  return parent;
}


/* Parse all the lines devoted to the next object in the database. */
void parse_obj(FILE *fh) {
  int objnumber;
  char *nameptr;
  int flags, owner, location, contents, next, parent, child, sibling;
  int verbdefs, propdefs, nprops;
  int i;
  char *vnameptr;
  int vowner, vperms, vprep;
  int dobj, iobj;

  char **propdefnames;
  struct property *propstructs;

  objnumber = read_obj(fh);
  if (mode == OBJECTS) {
    if (objnumber < 0) {
      printf("#%d recycled\n", -objnumber);
    } else {
      printf("#%d\n", objnumber);
    }
  }

  if (objnumber < 0) {
     if (mode_object == -objnumber)
       printf("Recycled\n");
     return;
  }

  nameptr = read_big_str(fh);
  read_junk_str(fh);
  read_int(flags, fh);
  read_int(owner, fh);
  read_int(location, fh);
  read_int(contents, fh);
  read_int(next, fh);
  read_int(parent, fh);
  read_int(child, fh);
  read_int(sibling, fh);
  if (mode == OBJECT && mode_object == objnumber) {
    printf("Name:\t%s\n", nameptr);
    printf("Flags:\t");
    if (flags & OB_USER) printf("player ");
    if (flags & OB_PROGRAMMER) printf("programmer ");
    if (flags & OB_WIZARD) printf("wizard ");
    /* if (flags & OB_OBSOLETE_1) printf(" X "); */
    if (flags & OB_READ) printf("r ");
    if (flags & OB_WRITE) printf("w ");
    /* if (flags & OB_OBSOLETE_2) printf(" Y "); */
    if (flags & OB_FERTILE) printf("f ");
    printf("\n");
    printf("Owner:\t#%d\n", owner);
    printf("Location:\t#%d\n", location);
    printf("Parent:\t#%d\n", parent);
  }
  free(nameptr);
  nameptr = NULL;
  if (mode == OBJECT && mode_object == parent) {
    printf("Child:\t#%d\n", objnumber);
  }
  if (mode == OBJECT && mode_object == location) {
    printf("Contains:\t#%d\n", objnumber);
  }
  if (mode == OBJECT && mode_object == owner) {
    printf("Owns:\t#%d\n", objnumber);
  }

  read_int(verbdefs, fh);
  for (i = 1; i <= verbdefs; i++) {
    /* Parse all the lines devoted to a verb definition */
    if (mode_object == objnumber)
      vnameptr = read_big_str(fh);
    else
      read_junk_str(fh);
    read_int(vowner, fh);
    read_int(vperms, fh);
    read_int(vprep, fh);
    if (mode == OBJECT && mode_object == objnumber) {
      printf("Verb %d:\t:%s\n", i-1, vnameptr);
    } else if (mode == VERB && mode_object == objnumber && mode_verb == i-1) {
      printf("Name:\t%s\n", vnameptr);
      printf("Owner:\t#%d\n", vowner);
      printf("Perms:\t");
      if (vperms & VF_READ) printf("r");
      if (vperms & VF_WRITE) printf("w");
      if (vperms & VF_EXEC) printf("x");
      if (vperms & VF_DEBUG) printf("d");
      if (!(vperms & VF_NOT_O)) printf("o");
      printf("\n");
      printf("Prepositions:\t");
      dobj = (vperms >> DOBJSHIFT) & OBJMASK;
      iobj = (vperms >> IOBJSHIFT) & OBJMASK;
      if (dobj == ASPEC_NONE)
        printf("none");
      else if (dobj == ASPEC_ANY)
        printf("any");
      else if (dobj == ASPEC_THIS)
        printf("this");
      else {
        printf("\nUnknown dobj: %d\n", dobj);
        exit(1);
      }
      printf(" ");
      if (vprep == PREP_ANY)
        printf("any");
      else if (vprep == PREP_NONE)
        printf("none");
      else if (0 <= vprep || vprep < sizeof(prep_list) / sizeof(*prep_list))
        printf(prep_list[vprep]);
      else {
        printf("\nUnknown prep: %d\n", vprep);
        exit(1);
      }
      printf(" ");
      if (iobj == ASPEC_NONE)
        printf("none");
      else if (iobj == ASPEC_ANY)
        printf("any");
      else if (iobj == ASPEC_THIS)
        printf("this");
      else {
        printf("\nUnknown iobj: %d\n", iobj);
        exit(1);
      }
      printf("\n");
    }
    if (mode_object == objnumber)
      free(vnameptr);
  }

  /*
  For most of this program we just scan through data and instantly print anything that looks interesting.
  Unlike the Moo, we don't soak up reams of data into memory.
  However, the inherited property data is spread all over the database.
  So we'll read it all in before printing it out.
  */
  read_int(propdefs, fh);
  if ((mode == OBJECT || mode == PROP) && mode_object == objnumber) {
    /* We are interested in the object.  Record all the property names. */
    propdefnames = malloc(propdefs * sizeof(char *));
    if (!propdefnames) {
      printf("Unable to allocate %u bytes of memory. (parse_obj: propdefnames)\n", propdefs * sizeof(char *));
      exit(1);
    }
    for (i = 1; i <= propdefs; i++)
      propdefnames[i-1] = read_big_str(fh);
  } else {
    /* We don't care about this object. */
    for (i = 1; i <= propdefs; i++)
      read_junk_str(fh);
  }

  read_int(nprops, fh);
  if ((mode == OBJECT || mode == PROP) && mode_object == objnumber) {
    /* Now that we know the total number of properties, build a structure to hold this info. */
    propstructs = malloc(nprops * sizeof(struct property));
    if (!propstructs) {
      printf("Unable to allocate %u bytes of memory. (parse_obj: propstructs)\n", nprops * sizeof(struct property));
      exit(1);
    }
    /* Copy over the defined list of prop names. */
    for (i = 1; i <= propdefs; i++) {
      propstructs[i-1].rootobj = objnumber;
      propstructs[i-1].name = propdefnames[i-1];
      propstructs[i-1].clear = 0;
    }
    free(propdefnames);
    /* Read in the value, owner and perms of all props. */
    for (i = 1; i <= nprops; i++) {
      propstructs[i-1].value = parse_value(1, fh);
      propstructs[i-1].clear = (strcmp(propstructs[i-1].value, "[clear]") == 0);
      read_int(propstructs[i-1].owner, fh);
      read_int(propstructs[i-1].perms, fh);
    }
    /* Look up all ancestors to obtain missing names and values. */
    i = propdefs-1;
    while (parent != -1) {
      parent = dig_props(parent, propstructs, &i);
    }
    /* Print and free the collected property data. */
    for (i = 1; i <= nprops; i++) {
      if (mode == OBJECT) {
        printf("Property %d:\t#%d.%s\n", i-1, propstructs[i-1].rootobj, propstructs[i-1].name);
      } else if (strcmp(mode_prop, propstructs[i-1].name) == 0) {
        printf("Defined:\t#%d\n", propstructs[i-1].rootobj);
        printf("Owner:\t#%d\n", propstructs[i-1].owner);
        printf("Perms:\t");
        if (propstructs[i-1].perms & PF_READ) printf("r");
        if (propstructs[i-1].perms & PF_WRITE) printf("w");
        if (propstructs[i-1].perms & PF_CHOWN) printf("c");
        printf("\n");
        printf("Clear:\t%d\n", propstructs[i-1].clear);
        printf("Value:\t%s\n", propstructs[i-1].value);
        /* We're done.  Quit and let the OS clean up our memory. */
        fclose(fh);
        exit(0);
      }
      free(propstructs[i-1].name);
      free(propstructs[i-1].value);
    }
    free(propstructs);
  } else {
    /* We don't care about this object. */
    for (i = 1; i <= nprops; i++) {
      free(parse_value(0, fh));
      read_int(vowner, fh);
      read_int(vperms, fh);
    }
  }
}


/* Read in all the lines for one verb. */
void parse_verb(FILE *fh) {
  char *lineptr;

  /* Read the object number and verb number (eg. "#123:4") */
  lineptr = read_big_str(fh);
  if (mode == VERB && strcmp(mode_verbstr, lineptr) == 0) {
    /* This is the verb we seek; print it out. */
    printf("\n");
    do {
      free(lineptr);
      lineptr = read_big_str(fh);
      printf("%s\n", lineptr);
    } while (strcmp(lineptr, ".") != 0);
  } else {
    free(lineptr);
    /* We're not interested in this verb, just eat it quickly. [optimised] */
    /* Note: This code assumes that no compiled Moo verb can have a line ending with a dot. */
    lineptr = (char *) malloc(sizeof(char) * 256);
    if (!lineptr) {
      printf("Unable to allocate %u bytes of memory (parse_verb).\n", sizeof(char) * 256);
      exit(1);
    }
    do
      fgets(lineptr, 256, fh);
    while (strcmp(lineptr, ".\n") != 0);
  }
  free(lineptr);
}


int main(int argc, char *argv[]) {
  FILE *fh;
  char *lineptr;
  int nobjs, nprogs, dummy, nusers, user, clocks;
  /* int queuedtasks, suspendedtasks, connections; */
  int i;

  if (argc == 1 || argc > 3) {
    printf("Usage: moobrowser <DB FILE>\n");
    printf("       moobrowser <DB FILE> [objects | players]\n");
    /* printf("       moobrowser <DB FILE> [objects | players | queuedtasks | suspendedtasks | connections]\n"); */
    printf("       moobrowser <DB FILE> [<OBJECT> | <OBJECT:VERBNUM> | <OBJECT.PROPNAME>]\n");
    printf("       Object numbers can omit the '#' symbol.\n");
    return 1;
  }

  if (argc == 2) {
    mode = SUMMARY;
    printf("> Summary\n");
  } else if (strcmp(argv[2], "objects") == 0) {
    mode = OBJECTS;
    printf("> Objects\n");
  } else if (strcmp(argv[2], "players") == 0) {
    mode = PLAYERS;
    printf("> Players\n");
  } else if (strcmp(argv[2], "clocks") == 0) {
    /* This is an undocumented feature.  The clock section is obsolete and should be empty. */
    mode = CLOCKS;
    printf("> Clocks\n");
  } else if (strcmp(argv[2], "queuedtasks") == 0) {
    mode = QUEUEDTASKS;
    printf("> Queued tasks\n");
  } else if (strcmp(argv[2], "suspendedtasks") == 0) {
    mode = SUSPENDEDTASKS;
    printf("> Suspended tasks\n");
  } else if (strcmp(argv[2], "connections") == 0) {
    mode = CONNECTIONS;
    printf("> Connections\n");
  } else {
    if (argv[2][0] == '#') {
      /* Eat the redundant # symbol. */
      strcpy(argv[2], argv[2]+1);
    }
    if (strchr(argv[2], '.')) {
      mode = PROP;
      mode_object = atoi(argv[2]);
      strncpy(mode_prop, (char *) strchr(argv[2], '.')+1, sizeof(mode_prop));
      printf("> #%d.%s\n", mode_object, mode_prop);
    } else if (strchr(argv[2], ':')) {
      mode = VERB;
      mode_object = atoi(argv[2]);
      strcpy(argv[2], (char *) strchr(argv[2], ':')+1);
      mode_verb = atoi(argv[2]);
      printf("> #%d:%d\n", mode_object, mode_verb);
      sprintf(mode_verbstr, "#%d:%d", mode_object, mode_verb);
    } else {
      mode = OBJECT;
      mode_object = atoi(argv[2]);
      printf("> #%d\n", mode_object);
    }
  }

  /* Save the filename into a global variable so that other functions can reopen it for deeper data. */
  databasename = (char *) malloc(sizeof(char) * (strlen(argv[1])+1));
  if (!databasename) {
    printf("Unable to allocate %u bytes of memory. (main: databasename)\n", sizeof(char) * (strlen(argv[1])+1));
    exit(1);
  }
  strcpy(databasename, argv[1]);

  /* Open file for input */
  fh = openfile(databasename);

  /* Parse the intro section */
  parse_intro(&lineptr, &nobjs, &nprogs, &dummy, &nusers, fh);
  if (mode == SUMMARY)
    printf("%s\n", lineptr);
  free(lineptr);
  lineptr = NULL;

  /* List of players */
  if (mode == PLAYERS || mode == SUMMARY)
    printf("Players: %d\n", nusers);
  for (i = 1; i <= nusers; i++) {
    read_int(user, fh);
    if (mode == PLAYERS)
      printf("#%d\n", user);
  }
  if (mode == PLAYERS) {
    fclose(fh);
    return 0;
  }

  /* List of objects */
  if (mode == OBJECTS || mode == SUMMARY)
    printf("Objects: %d\n", nobjs);
  for (i = 1; i <= nobjs; i++) {
    parse_obj(fh);
  }
  if (mode == OBJECTS || mode == PROP || mode == OBJECT) {
    fclose(fh);
    return 0;
  }

  /* List of verbs */
  if (mode == SUMMARY)
    printf("Verbs: %d\n", nprogs);
  for (i = 1; i <= nprogs; i++) {
    parse_verb(fh);
  }
  if (mode == VERB) {
    fclose(fh);
    return 0;
  }
  
  /* Normally there are 0 clocks (this whole section is obsolete) */
  read_int(clocks, fh);
  if (mode == CLOCKS)
    printf("Clocks: %d\n", clocks);
  for (i = 1; i <= clocks; i++) {
    lineptr = read_big_str(fh);
    if (mode == CLOCKS)
      printf("%s\n", lineptr);
    free(lineptr);
    lineptr = NULL;
  }
  if (mode == CLOCKS) {
    fclose(fh);
    return 0;
  }

  /*
  The following three sections of the database don't interest me.
  If they interest you, feel free to implement them.  -- Neil, 2005
  */

  /* Queued tasks */
  if (mode == QUEUEDTASKS)
    printf("Reporting of queued tasks has not been implemented yet.\n");
  /* Suspended tasks */
  if (mode == SUSPENDEDTASKS)
    printf("Reporting of suspended tasks has not been implemented yet.\n");
  /* Connections */
  if (mode == CONNECTIONS)
    printf("Reporting of connections has not been implemented yet.\n");

  fclose(fh);
  return 0;
}
