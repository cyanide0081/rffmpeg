#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      L"RFF"
#define PROGRAM_NAME_FULL L"RFFMPEG"
#define PROGRAM_VERSION   L"v0.4.1-beta"

static const wchar_t *fullTitle = PROGRAM_NAME_FULL L" " PROGRAM_VERSION L" d-_-b";
static const wchar_t *consoleWindowTitle = PROGRAM_NAME L" " PROGRAM_VERSION;

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define CHARCOLOR_WHITE      L"\x1b[37m"
#define CHARCOLOR_WHITE_BOLD L"\x1b[97m"
#define CHARCOLOR_RED        L"\x1b[31m"
#define CHARCOLOR_RED_BOLD   L"\x1b[91m"
#define COLOR_DEFAULT        L"\x1b[0m"

/* Buffers and size limitations */
#define BUFFER   (512)
#define SHORTBUF (BUFFER / 16)
#define LONGBUF  (BUFFER * 8)
#define PATHBUF  (MAX_PATH)

#define MAX_ARGS 4
#define MAX_OPTS 5

/* Argument and option arrays' index trackers */
#define ARG_INPATH       0
#define ARG_INFORMAT     1
#define ARG_INPARAMETERS 2
#define ARG_OUTFORMAT    3

#define OPT_DISPLAYHELP      0
#define OPT_MAKENEWFOLDER    1
#define OPT_DELETEOLDFILES   2
#define OPT_DISABLERECURSION 3
#define OPT_FORCEOVERWRITE   4

/* Option string constants */
#define OPT_DISPLAYHELP_STRING      L"--help"  
#define OPT_MAKENEWFOLDER_STRING    L"--newfolder"  
#define OPT_DELETEOLDFILES_STRING   L"--delete"  
#define OPT_DISABLERECURSION_STRING L"--norecursion"
#define OPT_FORCEOVERWRITE_STRING   L"--overwrite"

/* Misc functinality identifiers */
#define IDENTIFIER_NO_PATH   L"__ident_no_path"
#define IDENTIFIER_NO_FORMAT L"__identifier_no_format"

#endif