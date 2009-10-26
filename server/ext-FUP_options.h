

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

