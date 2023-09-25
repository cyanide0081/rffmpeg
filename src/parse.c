#include <parse.h>

#define expectToken(arg, tok) if (strcasecmp(arg, "-" tok) == 0)
#define expectCompositeToken(arg, tok) if (strstr(arg, "-" tok))

#define COMP_TOKEN_DELIM ":"

#define prompt(str) printf("%s > %s%s:%s ",           \
                           COLOR_ACCENT,              \
                           COLOR_DEFAULT,             \
                           str,                       \
                           COLOR_INPUT);

static char **_getTokenizedStrings(char *string, const char *delimiter);

/* Parses the argument strings from direct
 * console input in case no argument is given */
void parseConsoleInput(arguments *args) {
    char input[ARG_BUF] = {0};

    /* TODO: handle parsing of multiple paths with quoted strings
       instead of the current sloppy OS-specific delimiters*/
    prompt("Input path(s) (separated by a '" DIR_DELIMITER "')");
    readLine(input, ARG_BUF);
    printf(COLOR_DEFAULT);
    args->inPaths = _getTokenizedStrings(input, DIR_DELIMITER);

    prompt("Target format(s)");
    readLine(input, ARG_BUF);
    printf(COLOR_DEFAULT);
    args->inFormats = _getTokenizedStrings(input, ", ");

    prompt("FFmpeg options");
    readLine(input, ARG_BUF);
    printf(COLOR_DEFAULT);
    args->ffOptions = strdup(input);

    prompt("Output format");
    readLine(input, ARG_BUF);
    args->outFormat = strdup(input);
    printf(COLOR_DEFAULT);

    prompt("Additional flags");
    readLine(input, ARG_BUF);
    printf("%s\n", COLOR_DEFAULT);

    char **optionsList = _getTokenizedStrings(input, " ");
    size_t listSize = 0;

    while (optionsList[listSize])
        listSize++;

    parseArgs(listSize, optionsList, args);

    for (size_t i = 0; i < listSize; i++)
        free(optionsList[i]);

    free(optionsList);
}

/* Parses an array of strings to format an (arguments*) accordingly */
void parseArgs(const int listSize, char *args[], arguments *parsedArgs) {
    if (listSize == 0)
        return;

    size_t count =  listSize, parsedArgsIdx = 0;

    for (size_t i = 1; i < count && args[i]; i++) {
        expectToken(args[i], "-help") {
            parsedArgs->options |= OPT_DISPLAYHELP;
            return;
        }
        expectToken(args[i], "i") {
            parsedArgs->inFormats = _getTokenizedStrings(args[++i], ", ");
            continue;
        }

        expectToken(args[i], "p") {
            parsedArgs->ffOptions = strdup(args[++i]);
            continue;
        }

        expectToken(args[i], "o") {
            parsedArgs->outFormat = args[++i];
            continue;
        }

        expectToken(args[i], "cl") {
            parsedArgs->options |= OPT_CLEANUP;
            continue;
        }

        expectToken(args[i], "rn") {
            parsedArgs->options |= OPT_NORECURSION;
            continue;
        }

        expectToken(args[i], "ow") {
            parsedArgs->options |= OPT_OVERWRITE;
            continue;
        }

        expectCompositeToken(args[i], "subfolder") {
            parsedArgs->options |= OPT_NEWFOLDER;

            char *delimPoint = strstr(args[i], COMP_TOKEN_DELIM);

            if (delimPoint) {
                parsedArgs->options |= OPT_CUSTOMFOLDERNAME;
                parsedArgs->customFolder = strdup(++delimPoint);
            }

            continue;
        }

        expectCompositeToken(args[i], "outpath") {
            parsedArgs->options |= OPT_NEWPATH;

            char *delimPoint = strstr(args[i], COMP_TOKEN_DELIM);

            if (delimPoint) {
                parsedArgs->options |= OPT_CUSTOMPATHNAME;
                parsedArgs->customPath = strdup(++delimPoint);
            }

            continue;
        }

        if (isDirectory(args[i])) {
            parsedArgs->inPaths[parsedArgsIdx++] = strdup(args[i]);
        } else {
            printErr("unrecognized option", args[i]);
        }
    }

    /* handle null args here */
    if (!parsedArgs->ffOptions)
        parsedArgs->ffOptions = strdup("");

    if (!*parsedArgs->inFormats)
        *parsedArgs->inFormats = strdup("");

    if (!parsedArgs->customPath)
        parsedArgs->customPath = strdup("");
}

static char **_getTokenizedStrings(char *string, const char *delimiter) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);

    size_t items = LIST_BUF;
    char **list = xcalloc(items, sizeof(char*));

    if (!token) {
        *list = strdup("");
        return list;
    }
    size_t i;

    for (i = 0; token; i++) {
        if (i == items) {
            size_t newCount = items * 2;

            xrealloc(list, items * sizeof(char*));
            memset(list + items, 0, newCount - items);

            items = newCount;
        }

        trimSpaces(token);

        (list)[i] = strdup(token);

        token = strtok_r(NULL, delimiter, &parserState);
    }

    (list)[i] = NULL;
    return list;
}
