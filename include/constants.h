#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      u"RFF"
#define PROGRAM_NAME_FULL u"RFFMPEG"
#define PROGRAM_VERSION   u"v0.6.3-beta"

static const char16_t *fullTitle = PROGRAM_NAME_FULL u" " PROGRAM_VERSION u" (ユニコード)";
static const char16_t *consoleWindowTitle = PROGRAM_NAME u" " PROGRAM_VERSION;

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define CHARCOLOR_WHITE      u"\x1b[37m"
#define CHARCOLOR_WHITE_BOLD u"\x1b[97m"
#define CHARCOLOR_RED        u"\x1b[31m"
#define CHARCOLOR_RED_BOLD   u"\x1b[91m"
#define COLOR_DEFAULT        u"\x1b[0m"

/* Buffers and size limitations */
#define BUFFER   (512)
#define SHORTBUF (BUFFER / 16)
#define LONGBUF  (BUFFER * 8)
#define PATHBUF  (MAX_PATH)

/* Optional flags' constants */
#define OPT_DISPLAYHELP_STRING      u"--help"  
#define OPT_MAKENEWFOLDER_STRING    u"--newfolder"  
#define OPT_DELETEOLDFILES_STRING   u"--delete"  
#define OPT_DISABLERECURSION_STRING u"--norecursion"
#define OPT_FORCEOVERWRITE_STRING   u"--overwrite"

/* Misc functinality identifiers */
#define IDENTIFIER_NO_PATH   u"__ident_no_path"
#define IDENTIFIER_NO_FORMAT u"__identifier_no_format"

#endif