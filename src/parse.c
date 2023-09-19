#include "data.h"
#include <parse.h>

#define expectToken(arg, tok) if (strcasecmp(arg, "-" tok) == 0)
#define expectCompositeToken(arg, tok) if (strstr(arg, "-" tok))

#define prompt(str) printf("%s > %s%s:%s ",\
                           CHARCOLOR_RED,             \
                           CHARCOLOR_WHITE,           \
                           str,                       \
                           CHARCOLOR_WHITE_BOLD);


static char **_tokenizeArguments(char *string, const char *delimiter);

/* Parses the argument strings from direct
 * console input in case no argument is given */
void parseConsoleInput(arguments *args) {
    char *input = NULL;
    size_t len = 0;

    /* TODO: handle parsing of multiple paths with quoted strings
       instead of the current sloppy OS-specific delimiters*/
    prompt("Input path(s)");
    getline(&input, &len, stdin);
    trimSpaces(input);

    args->inPaths = _tokenizeArguments(input, DIR_DELIMITER);
    free(input);

    prompt("Target format(s)");
    getline(&input, &len, stdin);
    trimSpaces(input);

    args->inFormats = _tokenizeArguments(input, ", ");
    free(input);

    prompt("FFmpeg options");
    getline(&args->ffOptions, &len, stdin);
    trimSpaces(args->ffOptions);

    prompt("Output format");
    getline(&args->outFormat, &len, stdin);
    trimSpaces(args->outFormat);

    prompt("Additional flags");
    getline(&input, &len, stdin);
    trimSpaces(input);

    printf("%s\n", COLOR_DEFAULT);

    char **optionsList = _tokenizeArguments(input, ", ");
    parseArgs(0, optionsList, args);

    for (int i = 0; optionsList[i] != NULL; i++)
        free(optionsList[i]);

    free(optionsList);
    free(input);
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

            char *delimPoint = strstr(args[i], "=");

            if (delimPoint) {
                parsedArgs->customPathName = strdup(++delimPoint);
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
