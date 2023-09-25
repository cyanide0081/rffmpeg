#ifndef H_PARSERS_P
#define H_PARSERS_P

#ifdef __linux__
#include <linux/limits.h>
#endif

#include <libs.h>
#include <data.h>

/* Option bitmasks */
#define OPT_DISPLAYHELP        0x01
#define OPT_NORECURSION        0x02
#define OPT_OVERWRITE          0x04
#define OPT_CLEANUP            0x08
#define OPT_NEWFOLDER          0x10
#define OPT_CUSTOMFOLDERNAME   0x20
#define OPT_NEWPATH            0x40
#define OPT_CUSTOMPATHNAME     0x80

#ifdef _WIN32
#define DIR_DELIMITER ";"
#else
#define DIR_DELIMITER ":"
#endif

void parseConsoleInput(arguments *args);

void parseArgs(const int count,
               char *rawArguments[],
               arguments *parsedArguments);

#endif // H_PARSERS_P
