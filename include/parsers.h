#ifndef H_PARSERS_P
#define H_PARSERS_P

#include "libs.h"
#include "types.h"
#include "terminal.h"

#include <string.h>

/* Argument identifiers' constants */
#define ARG_INPUTPATHS         "-path"
#define ARG_INPUTFORMATS       "-in"
#define ARG_INPUTPARAMETERS    "-opts"
#define ARG_OUTPUTFORMAT       "-out"

/* Optional flags' constants */
#define OPT_DISPLAYHELP_STRING "--help"  
#define OPT_NEWFOLDER_STRING   "--newfolder"  
#define OPT_CLEANUP_STRING     "--cleanup"  
#define OPT_NORECURSION_STRING "--norecursion"
#define OPT_OVERWRITE_STRING   "--overwrite"

/* Option bitmasks */
#define OPT_DISPLAYHELP        0x01
#define OPT_NORECURSION        0x02
#define OPT_OVERWRITE          0x04
#define OPT_CLEANUP            0x08
#define OPT_NEWFOLDER          0x10
#define OPT_CUSTOMFOLDERNAME   0x20
// #define OPT_NEWPATH            0x40
// #define OPT_CUSTOMPATHNAME     0x80
/* ^^^ UNIMPLEMENTED ^^^ */

int parseConsoleInput(arguments *arguments);

int parseArguments(const int count, char *rawArguments[], arguments *parsedArguments);

#endif // H_PARSERS_P