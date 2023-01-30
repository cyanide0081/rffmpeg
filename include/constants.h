#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      "RFF"
#define PROGRAM_NAME_FULL "RFFMPEG"
#define PROGRAM_VERSION   "v0.6.0-beta"

static const char *fullTitle = PROGRAM_NAME_FULL " " PROGRAM_VERSION u8" (ユニコード)";
static const char *consoleWindowTitle = PROGRAM_NAME " " PROGRAM_VERSION;

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define CHARCOLOR_WHITE      "\x1b[37m"
#define CHARCOLOR_WHITE_BOLD "\x1b[97m"
#define CHARCOLOR_RED        "\x1b[31m"
#define CHARCOLOR_RED_BOLD   "\x1b[91m"
#define COLOR_DEFAULT        "\x1b[0m"

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
#define OPT_DISPLAYHELP_STRING      "--help"  
#define OPT_MAKENEWFOLDER_STRING    "--newfolder"  
#define OPT_DELETEOLDFILES_STRING   "--delete"  
#define OPT_DISABLERECURSION_STRING "--norecursion"
#define OPT_FORCEOVERWRITE_STRING   "--overwrite"

/* Misc functinality identifiers */
#define IDENTIFIER_NO_PATH   "__ident_no_path"
#define IDENTIFIER_NO_FORMAT "__identifier_no_format"

#endif