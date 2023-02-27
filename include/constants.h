#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      "RFF"
#define PROGRAM_NAME_FULL "RFFMPEG"
#define PROGRAM_VERSION   "v1.0.0"

#define FULL_PROGRAM_TITLE   (PROGRAM_NAME_FULL " " PROGRAM_VERSION " (跨平台)")
#define CONSOLE_WINDOW_TITLE (PROGRAM_NAME " " PROGRAM_VERSION)

/* ANSI escape chars for colored shell output (Virtual Terminal Sequences) */
#define COLOR_DEFAULT        "\x1b[0m"
#define CHARCOLOR_RED        "\x1b[31m"
#define CHARCOLOR_WHITE      "\x1b[37m"
#define CHARCOLOR_RED_BOLD   "\x1b[91m"
#define CHARCOLOR_WHITE_BOLD "\x1b[97m"

#define LIST_BUFFER 8 // Reasonably large initial buffer for number of list elements

#if defined _WIN32
    #define FILE_BUFFER MAX_PATH
    #define PATH_BUFFER MAX_PATH
#else
    #define FILE_BUFFER NAME_MAX
    #define PATH_BUFFER PATH_MAX
#endif

#endif // H_CONSTS