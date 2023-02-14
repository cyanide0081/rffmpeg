#ifndef H_PARSERS
#define H_PARSERS

#include "libs.h"
#include "types.h"
#include "terminal.h"

/* Argument identifiers' constants */
#define ARG_INPUTPATHS         u"-path"
#define ARG_INPUTFORMATS       u"-in"
#define ARG_INPUTPARAMETERS    u"-opts"
#define ARG_OUTPUTFORMAT       u"-out"

/* Optional flags' constants */
#define OPT_DISPLAYHELP_STRING u"--help"  
#define OPT_NEWFOLDER_STRING   u"--newfolder"  
#define OPT_CLEANUP_STRING     u"--cleanup"  
#define OPT_NORECURSION_STRING u"--norecursion"
#define OPT_OVERWRITE_STRING   u"--overwrite"

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

int parseArguments(const int count, char16_t *rawArguments[], arguments *parsedArguments);

#endif // H_PARSERS