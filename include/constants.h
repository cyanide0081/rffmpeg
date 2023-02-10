#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      u"RFF"
#define PROGRAM_NAME_FULL u"RFFMPEG"
#define PROGRAM_VERSION   u"v0.6.6-beta"

static const char16_t *fullTitle = PROGRAM_NAME_FULL u" " PROGRAM_VERSION u" (ユニコード)";
static const char16_t *consoleWindowTitle = PROGRAM_NAME u" " PROGRAM_VERSION;

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define COLOR_DEFAULT        u"\x1b[0m"
#define CHARCOLOR_RED        u"\x1b[31m"
#define CHARCOLOR_WHITE      u"\x1b[37m"
#define CHARCOLOR_RED_BOLD   u"\x1b[91m"
#define CHARCOLOR_WHITE_BOLD u"\x1b[97m"

/* Buffers and size limitations */
#define SHORTBUF (64)
#define BUFFER   (512)
#define LONGBUF  (1024)
#define PATHBUF  (MAX_PATH)

/* Option bitmasks */
#define OPT_DISPLAYHELP         0x01
#define OPT_MAKENEWFOLDER       0x02
#define OPT_CUSTOMFOLDERNAME    0x04
#define OPT_DISABLERECURSION    0x08
#define OPT_FORCEFILEOVERWRITES 0x10
#define OPT_DELETEORIGINALFILES 0x20

/* Argument identifiers' constants */
#define ARG_INPUTPATHS      u"-path"
#define ARG_INPUTFORMATS    u"-in"
#define ARG_INPUTPARAMETERS u"-opts"
#define ARG_OUTPUTFORMAT    u"-out"

/* Optional flags' constants */
#define OPT_DISPLAYHELP_STRING      u"--help"  
#define OPT_MAKENEWFOLDER_STRING    u"--newfolder"  
#define OPT_DELETEOLDFILES_STRING   u"--cleanup"  
#define OPT_DISABLERECURSION_STRING u"--norecursion"
#define OPT_FORCEOVERWRITE_STRING   u"--overwrite"

/* Empty string identifiers */
#define IDENTIFIER_NO_PATH   u"___IDENTIFIER_NO_PATH___"
#define IDENTIFIER_NO_FORMAT u"___IDENTIFIER_NO_FORMAT___"

#define ERROR_NO_INPUT_FORMAT          81000
#define ERROR_NO_OUTPUT_FORMAT         81001
#define ERROR_FAILED_SETCONSOLEMODE    81002
#define ERROR_FAILED_TO_OPEN_DIRECTORY 81003

#endif