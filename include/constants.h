#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      "RFF"
#define PROGRAM_NAME_FULL "RFFMPEG"
#define PROGRAM_VERSION   "v0.7.1-beta"

static const char *fullTitle = PROGRAM_NAME_FULL " " PROGRAM_VERSION " (ユニコード)";
static const char *consoleWindowTitle = PROGRAM_NAME " " PROGRAM_VERSION;

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define COLOR_DEFAULT        "\x1b[0m"
#define CHARCOLOR_RED        "\x1b[31m"
#define CHARCOLOR_WHITE      "\x1b[37m"
#define CHARCOLOR_RED_BOLD   "\x1b[91m"
#define CHARCOLOR_WHITE_BOLD "\x1b[97m"

/* Buffers for string handling */
#define SHORTBUF (64)
#define BUFFER   (1024)
#define LONGBUF  (4096)

#if defined _WIN32
    #define FILE_BUFFER (MAX_PATH)
    #define PATH_BUFFER (MAX_PATH)
#else
    #define FILE_BUFFER (UINT8_MAX)
    #define PATH_BUFFER (512) /* An arbitrary initial buffer for a theoretically unlimited path size */
#endif

#endif // H_CONSTS