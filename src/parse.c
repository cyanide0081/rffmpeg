#include <parse.h>

#define expectToken(arg, tok) if (strcasecmp(arg, "-" tok) == 0)
#define expectCompositeToken(arg, tok) if (strstr(arg, "-" tok))

static char **_tokenizeArguments(char *string, const char *delimiter);

/* Parses the argument strings from direct
 * console input in case no argument is given */
void parseConsoleInput(arguments *args) {
    char *inputPathsString = NULL;
    size_t inputPathsSize = 0;

    /* TODO: handle parsing of multiple paths with quoted strings
       instead of the current sloppy OS-specific delimiters*/
    printf("%s > %sInput path(s): %s",
           CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputPathsString, &inputPathsSize, stdin);
    trimSpaces(inputPathsString);

    args->inPaths = _tokenizeArguments(inputPathsString, DIR_DELIMITER);

    free(inputPathsString);

    char *inputFormatsString = NULL;
    size_t inputFormatsSize = 0;

    printf("%s > %sTarget format(s): %s",
           CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&inputFormatsString, &inputFormatsSize, stdin);
    trimSpaces(inputFormatsString);

    args->inFormats = _tokenizeArguments(inputFormatsString, ", ");

    free(inputFormatsString);

    size_t ffOptionsSize = 0;

    printf("%s > %sFFmpeg options: %s",
           CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&args->ffOptions, &ffOptionsSize, stdin);
    trimSpaces(args->ffOptions);

    size_t outFormatSize = 0;

    printf("%s > %sOutput extension: %s",
           CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&args->outFormat, &outFormatSize, stdin);
    trimSpaces(args->outFormat);

    char *optionsString = NULL;
    size_t optionsStringSize = 0;

    printf("%s > %sAdditional flags: %s",
           CHARCOLOR_RED, CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD);

    getline(&optionsString, &optionsStringSize, stdin);
    trimSpaces(optionsString);

    printf("\n");
    printf(COLOR_DEFAULT);

    char **optionsList = _tokenizeArguments(optionsString, ", ");
    parseArgs(0, optionsList, args);

    for (int i = 0; optionsList[i] != NULL; i++)
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
            parsedArgs->inFormats = _tokenizeArguments(args[++i], ", ");
            continue;
        }

        expectToken(args[i], "p") {
            parsedArgs->ffOptions = strdup(args[++i]);
            continue;
        }

        expectToken(args[i], "o") {
            parsedArgs->outFormat = strdup(args[++i]);
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

            char *delimiterSection = strstr(args[i], "=");

            if (delimiterSection != NULL) {
                parsedArgs->options |= OPT_CUSTOMFOLDERNAME;

                parsedArgs->customFolderName = strdup(++delimiterSection);
            }

            continue;
        }

        expectCompositeToken(args[i], "newpath") {
            parsedArgs->options |= OPT_NEWPATH;

            char *delimiterSection = strstr(args[i], "=");

            if (delimiterSection != NULL) {
                parsedArgs->customPathName = strdup(++delimiterSection);
            }

            continue;
        }

        parsedArgs->inPaths[parsedArgsIdx++] = strdup(args[i]);
    }
}

static char **_tokenizeArguments(char *string, const char *delimiter) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);

    size_t items = LIST_BUF;
    char **list = xcalloc(items, sizeof(char*));

    size_t i;

    for (i = 0; token; i++) {
        if (i == items) {
            size_t newCount = items * 2;

            list = realloc(list, items * sizeof(char*));
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
