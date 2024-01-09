#include <parse.h>

extern Arena *globalArena;

#define expectToken(arg, tok) if (strcasecmp(arg, "-" tok) == 0)
#define expectCompositeToken(arg, tok) if (strstr(arg, "-" tok))

#define COMP_TOKEN_DELIM ":"

#define prompt(str) printf("%s > %s%s:%s ",         \
    COLOR_ACCENT, COLOR_DEFAULT, str, COLOR_INPUT); \

static char **_getTokenizedStrings(char *string, const char *delimiter);

int parseConsoleInput(Arguments *args) {
    char input[ARG_BUF] = {0};

    prompt("Input path(s)");
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
    args->ffOptions = GlobalArenaPushString(input);

    prompt("Output format");
    readLine(input, ARG_BUF);
    args->outFormat = GlobalArenaPushString(input);
    printf(COLOR_DEFAULT);

    prompt("Additional flags");
    readLine(input, ARG_BUF);
    printf("%s\n", COLOR_DEFAULT);

    char **optionsList = _getTokenizedStrings(input, " ");
    size_t listSize = 0;

    while (optionsList[listSize]) listSize++;

    return parseArgs(listSize, optionsList, args);
}

/* Parses an array of strings to format an (arguments*) accordingly */
int parseArgs(const int listSize, char *args[], Arguments *parsedArgs) {
    if (listSize == 0) return PARSE_STATE_EMPTY;

    size_t count = listSize, parsedArgsIdx = 0;

    for (size_t i = 0; i < count; i++) {
        expectToken(args[i], "-help") {
            parsedArgs->options |= OPT_DISPLAYHELP;
            return PARSE_STATE_OK;
        }

        expectToken(args[i], "-version") {
            parsedArgs->options |= OPT_DISPLAYVERSION;
            return PARSE_STATE_OK;
        }

        /* (making sure we don't try to read out-of-bounds memory here) */
        if (i < count - 1) {
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
                parsedArgs->outPath.customFolder =
                    GlobalArenaPushString(++delimPoint);
            }

            continue;
        }

        expectCompositeToken(args[i], "outpath") {
            parsedArgs->options |= OPT_NEWPATH;

            char *delimPoint = strstr(args[i], COMP_TOKEN_DELIM);

            if (!delimPoint || !*(++delimPoint)) {
                printErr(
                    "missing custom path name after flag",
                    "(USAGE: -outpath:[PATH])"
                );
                exit(-1);
            }

            parsedArgs->options |= OPT_CUSTOMPATHNAME;
            parsedArgs->outPath.customPath = GlobalArenaPushString(delimPoint);

            continue;
        }

        expectCompositeToken(args[i], "nt") {
            char *delimPoint = strstr(args[i], COMP_TOKEN_DELIM);

            if (!delimPoint || !*(++delimPoint)) {
                printErr("missing number of threads after flag",
                    "(USAGE: -nt:[THREADS])");
                exit(EXIT_FAILURE);
            }

            errno = 0;

            long numberOfThreads = strtol(delimPoint, NULL, 10);

            if (errno || numberOfThreads < 1) {
                printErr("invalid custom thread number", delimPoint);
                exit(errno);
            }

            parsedArgs->numberOfThreads = (size_t)numberOfThreads;

            size_t maxThreads = (2 * getNumberOfOnlineThreads());

            if (parsedArgs->numberOfThreads > maxThreads) {
                fprintf(stderr,
                    " %sERROR: %sabove-limit custom thread number: "
                    "%s%zu %s(%smaximum value%s: %s%zu%s)\n\n",
                    COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                    parsedArgs->numberOfThreads, COLOR_DEFAULT,
                    COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                    maxThreads, COLOR_DEFAULT);
                exit(EXIT_FAILURE);
            }


            continue;
        }

        if (!isDirectory(args[i])) {
            printErr("invalid/incomplete option", args[i]);
            printf(" (run with %s--help%s for info)\n\n",
                COLOR_INPUT, COLOR_DEFAULT);

            return PARSE_STATE_INVALID;
        }

        char *absPath = getAbsolutePath(args[i]);
        bool isDupPath = false;

        for (size_t i = 0; i < parsedArgsIdx; i++) {
            if (strcmp(parsedArgs->inPaths[i], absPath) == 0) {
                isDupPath = true;
                break;
            }
        }

        if (isDupPath) {
            printf(" %sWARNING: %signoring duplicate path: %s\"%s\"%s\n\n",
                COLOR_ACCENT, COLOR_DEFAULT, COLOR_INPUT,
                absPath, COLOR_DEFAULT);

            continue;
        }

        parsedArgs->inPaths[parsedArgsIdx++] = absPath;
    }

    /* handle null args here */
    if (!parsedArgs->ffOptions) {
        parsedArgs->ffOptions = GlobalArenaPushString("");
    }

    if (!*parsedArgs->inFormats) {
        *parsedArgs->inFormats = GlobalArenaPushString("");
    }

    if (!parsedArgs->outPath.customPath) {
        parsedArgs->outPath.customPath = GlobalArenaPushString("");
    }

    /* Set current working directory as input path if none is provided */
    if (!parsedArgs->inPaths[0] || !*parsedArgs->inPaths[0]) {
        parsedArgs->inPaths[0] = getCurrentWorkingDirectory();
    }

    if (!parsedArgs->inFormats[0] || !*parsedArgs->inFormats[0]) {
        printErr("missing input format", "(null)");
        printf(" (run with %s--help%s for info)\n\n",
            COLOR_INPUT, COLOR_DEFAULT);

        return PARSE_STATE_BAD_ARG;
    }

    if (!parsedArgs->outFormat || !*parsedArgs->outFormat) {
        printErr("missing output format", "(null)");
        printf(" (run with %s--help%s for info)\n\n",
            COLOR_INPUT, COLOR_DEFAULT);

        return PARSE_STATE_BAD_ARG;
    }

    if (
        (parsedArgs->options & OPT_NEWFOLDER) &&
        (parsedArgs->options & OPT_NEWPATH)
    ) {
        printErr("using multiple unique switches",
            "('-subfolder' and '-outpath' are mutually exclusive)");

        return PARSE_STATE_BAD_ARG;
    }

    if (
        (parsedArgs->options & OPT_NEWFOLDER) &&
        (parsedArgs->options & OPT_CUSTOMFOLDERNAME)
    ) {
        if ((strlen(parsedArgs->outPath.customFolder) > FILE_BUF - 1)) {
            char len[FILE_BUF];
            snprintf(len, sizeof(len), "(%d bytes)", FILE_BUF);
            printErr("custom folder name exceeds maximum allowed length", len);

            return PARSE_STATE_LONG_ARG;
        }
    }

    for (int i = 0; parsedArgs->inFormats[i] != NULL; i++) {
        if (
            strcmp(parsedArgs->inFormats[i], parsedArgs->outFormat) == 0 &&
            !(parsedArgs->options & OPT_NEWFOLDER) &&
            !(parsedArgs->options & OPT_NEWPATH)
        ) {
            printErr("can't use ffmpeg with identical input and output formats",
                "(use '-outpath' or '-subfolder' to save the files in a new"
                " directory)");

            return PARSE_STATE_EXT_CONFLICT;
        }
    }

    return PARSE_STATE_OK;
}

static char **_getTokenizedStrings(char *string, const char *delimiter) {
    char *parserState = NULL;
    char *token = strtok_r(string, delimiter, &parserState);
    size_t items = LIST_BUF;
    char **list = GlobalArenaPush(items * sizeof(char*));

    if (!token) {
        *list = GlobalArenaPushString("");
        return list;
    }

    size_t i;

    for (i = 0; token; i++) {
        if (i == items) {
            size_t newCount = items * 2;

            char **newList = GlobalArenaPush(newCount * sizeof(char));
            memcpy(newList, list, items);
            memset(newList + items, 0, newCount - items);

            items = newCount;
        }

        trimSpaces(token);
        (list)[i] = GlobalArenaPushString(token);
        token = strtok_r(NULL, delimiter, &parserState);
    }

    (list)[i] = NULL;
    return list;
}
