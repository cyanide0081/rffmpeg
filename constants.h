#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      u8"RFF"
#define PROGRAM_NAME_FULL u8"RFFMPEG"
#define PROGRAM_VERSION   u8"v0.5.0-beta"

static const char *fullTitle = PROGRAM_NAME_FULL u8" " PROGRAM_VERSION u8" d-_-b";
static const char *consoleWindowTitle = PROGRAM_NAME u8" " PROGRAM_VERSION;

#define CODEPAGE_UTF8 65001

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define CHARCOLOR_WHITE      u8"\x1b[37m"
#define CHARCOLOR_WHITE_BOLD u8" \x1b[97m"
#define CHARCOLOR_RED        u8"\x1b[31m"
#define CHARCOLOR_RED_BOLD   u8"\x1b[91m"
#define COLOR_DEFAULT        u8"\x1b[0m"

/* Buffers and size limitations */
#define BUFFER   (512)
#define SHORTBUF (BUFFER / 16)
#define LONGBUF  (BUFFER * 8)
#define PATHBUF  (MAX_PATH)

#define MAX_ARGS 5
#define MAX_OPTS 6

/* Argument and option arrays' index trackers */
#define ARG_INPATH        0
#define ARG_INFORMAT      1
#define ARG_INPARAMETERS  2
#define ARG_OUTFORMAT     3
#define ARG_NEWFOLDERNAME 4

#define OPT_DISPLAYHELP      0
#define OPT_MAKENEWFOLDER    1
#define OPT_DELETEOLDFILES   2
#define OPT_DISABLERECURSION 3
#define OPT_FORCEOVERWRITE   4
#define OPT_CUSTOMFOLDERNAME 5

/* Option string constants */
#define OPT_DISPLAYHELP_STRING      u8"--help"  
#define OPT_MAKENEWFOLDER_STRING    u8"--newfolder"  
#define OPT_DELETEOLDFILES_STRING   u8"--delete"  
#define OPT_DISABLERECURSION_STRING u8"--norecursion"
#define OPT_FORCEOVERWRITE_STRING   u8"--overwrite"

/* Misc functinality identifiers */
#define IDENTIFIER_NO_PATH   u8"__ident_no_path"
#define IDENTIFIER_NO_FORMAT u8"__identifier_no_format"

#endif