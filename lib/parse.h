#ifndef H_PARSERS_P
#define H_PARSERS_P

#include <data.h>

/* Option bitmasks */
#define OPT_DISPLAYHELP        0x001
#define OPT_NORECURSION        0x002
#define OPT_OVERWRITE          0x004
#define OPT_CLEANUP            0x008
#define OPT_NEWFOLDER          0x010
#define OPT_CUSTOMFOLDERNAME   0x020
#define OPT_NEWPATH            0x040
#define OPT_CUSTOMPATHNAME     0x080
#define OPT_DISPLAYVERSION     0x100

#define PARSE_STATE_OK             0
#define PARSE_STATE_EMPTY          1
#define PARSE_STATE_INVALID        2
#define PARSE_STATE_CWD_ERROR      3
#define PARSE_STATE_BAD_ARG        4
#define PARSE_STATE_LONG_ARG       5
#define PARSE_STATE_EXT_CONFLICT   6

#ifdef _WIN32
#define DIR_DELIMITER ";"
#else
#define DIR_DELIMITER ":"
#endif

int parseConsoleInput(Arguments *args);

int parseArgs(
    const int count, char *rawArguments[], Arguments *parsedArguments
);

#endif // H_PARSERS_P
