/******************************************************************************
  Copyright (c) 1994,1995,1996 Weizmann Institute. All rights reserved.
  This is version 1.8 of the File Utilities Package (FUP), suitable for use
  with all versions of MOO1.8.0, up to beta2 inclusive.
  This file includes routines for file management and direct file access
  developed and written by Jaime Prilusky and Gustavo Glusman, Weizmann
  Institute of Science, Israel.
  This software is intended to work with the MOO server written by
  Pavel Curtis, Xerox PARC (Pavel@Xerox.Com), depending on routines
  and code from the MOO server itself.
  For questions, suggestions  and comments, please  send email to
  lsprilus@weizmann.weizmann.ac.il (Jaime Prilusky)
  Gustavo@bioinformatics.weizmann.ac.il (Gustavo Glusman)

  filemkdir and filermdir in collaboration with Jeremy Cooper <jeremy@crl.com>
  Thanks to Alex Stewart for valuable suggestions for improvements.
 *****************************************************************************/


/*************************************************************************
By default, the filechmod() and filerun() builtins are disabled. To enable 
any of them, uncomment the lines:
*/
     /* #define INCLUDE_FILERUN */
     /* #define INCLUDE_FILECHMOD */


/*******************************************************************
You can change the name of the base directory for the external file
hierarchy, which is 'hard-coded' into the server upon compilation.
To change it from its default ('files'), edit the line:
*/
#define EXTERN_FILES_DIR   "files/"


/***************************************************************************
The default name for the directory that holds the executables is defined in:
*/
#define EXTERN_BIN_DIR     "bin/"


/******************************************************************************
You can also make this base directory either read-only or read-and-write.
It is read-only by default. To make it writable from the MOO, comment the line:
*/
//     #define EXTERN_FILES_DIR_READ_ONLY


/**********************************************************
The default mode for new directories can be configured too:
*/
#define CREATE_NEW_DIR_MODE		0755

#define INCLUDE_FILECHMOD
#define INCLUDE_FILERUN
