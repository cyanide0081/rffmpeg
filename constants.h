#ifndef H_CONSTS
#define H_CONSTS

#define PROGRAM_NAME      "RFF"
#define PROGRAM_NAME_FULL "RFFMPEG"
#define PROGRAM_VERSION   "0.2 [ALPHA]"

static const char *full_title = PROGRAM_NAME_FULL " v" PROGRAM_VERSION " d-_-b";

#define CHARCOLOR_WHITE   "\033[37m"
#define CHARCOLOR_WHITE_B "\033[97m"
#define CHARCOLOR_RED     "\033[31m"
#define CHARCOLOR_RED_B   "\033[91m"
#define COLOR_DEFAULT     "\033[0m"

#define IDENTIFIER_NO_PATH "__ident_no_path"

/* sketchy commands (prohibited by richard stallman) */
static const char *cmd_sleep = "TIMEOUT /T 1 > NUL";
static const char *cmd_pause = "TIMEOUT /T -1 > NUL";

static const int _codepage = 65001;

#define BUFFER   (512 * sizeof(char))
#define SHORTBUF (BUFFER / 4)
#define LONGBUF  (BUFFER * 8)

#define MAX_ARGS (4 + 1)
#define MAX_OPTS (5 + 1)

#define ARG_PATH   0
#define ARG_FORMAT 1
#define ARG_PARAMS 2
#define ARG_OUTPUT 3

#define OPT_HELP      0
#define OPT_NEWFOLDER 1
#define OPT_DELETE    2
#define OPT_RECURSIVE 3
#define OPT_OVERWRITE 4

#endif