#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      L"RFF"
#define PROGRAM_NAME_FULL L"RFFMPEG"
#define PROGRAM_VERSION   L"v0.3.1-alpha"

static const wchar_t *fullTitle = PROGRAM_NAME_FULL L" " PROGRAM_VERSION L" d-_-b";
static const uint16_t _utf8Codepage = 65001;

/* ANSI eschape chars for colored shell output */
#define CHARCOLOR_WHITE      L"\033[37m"
#define CHARCOLOR_WHITE_BOLD L"\033[97m"
#define CHARCOLOR_RED        L"\033[31m"
#define CHARCOLOR_RED_BOLD   L"\033[91m"
#define COLOR_DEFAULT        L"\033[0m"

/* Buffers and size limitations */
#define BUFFER   (512 * sizeof(wchar_t))
#define SHORTBUF (BUFFER / 16)
#define LONGBUF  (BUFFER * 8)
#define PATHBUF  (MAX_PATH * sizeof(wchar_t))

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