#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      u"RFF"
#define PROGRAM_NAME_FULL u"RFFMPEG"
#define PROGRAM_VERSION   u"v0.7.1-beta"

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
#define BUFFER   (1024)
#define LONGBUF  (4096)

#if defined _WIN32
    #define PATH_BUFFER (MAX_PATH)  
#else
    #define PATH_BUFFER (UINT8_MAX)
#endif

#endif // H_CONSTS