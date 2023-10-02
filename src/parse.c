#include <parse.h>
#include <wchar.h>

#define expectToken(arg, tok) if (strcasecmp(arg, "-" tok) == 0)
#define expectCompositeToken(arg, tok) if (strstr(arg, "-" tok))

#define COMP_TOKEN_DELIM ":"

#define prompt(str) printf("%s > %s%s:%s ",           \
                           COLOR_ACCENT,              \
                           COLOR_DEFAULT,             \
                           str,                       \
                           COLOR_INPUT);

static char **_getTokenizedStrings(char *string, const char *delimiter);
static char *getAbsolutePath(const char *dir);

/* Parses the argument strings from direct
 * console input in case no argument is given */
int parseConsoleInput(arguments *args) {
    char input[ARG_BUF] = {0};

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

    int state = parseArgs(listSize, optionsList, args);

    for (size_t i = 0; i < listSize; i++)
        free(optionsList[i]);

    free(optionsList);

    return state;
}

/* Parses an array of strings to format an (arguments*) accordingly */
int parseArgs(const int listSize, char *args[], arguments *parsedArgs) {
    if (listSize == 0)
        return PARSE_STATE_EMPTY;

    size_t count =  listSize, parsedArgsIdx = 0;

    for (size_t i = 1; i < count && args[i]; i++) {
        expectToken(args[i], "-help") {
            parsedArgs->options |= OPT_DISPLAYHELP;
            return PARSE_STATE_OK;
        }

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
            parsedArgs->inPaths[parsedArgsIdx++] = getAbsolutePath(args[i]);
        } else {
            printErr("invalid/incomplete option", args[i]);
            printf(" (run with %s--help%s for info)\n\n",
                   COLOR_INPUT, COLOR_DEFAULT);

            return PARSE_STATE_INVALID;
        }
    }

    /* handle null args here */
    if (!parsedArgs->ffOptions)
        parsedArgs->ffOptions = strdup("");

    if (!*parsedArgs->inFormats)
        *parsedArgs->inFormats = strdup("");

    if (!parsedArgs->customPath)
        parsedArgs->customPath = strdup("");

    /* Set current working directory as input path if none is provided */
    if (!parsedArgs->inPaths[0] || !*parsedArgs->inPaths[0]) {
#ifdef _WIN32
        int len = GetCurrentDirectoryW(0, NULL);
        wchar_t *currentDirW = xcalloc(len, sizeof(wchar_t));
        GetCurrentDirectoryW((DWORD)len, currentDirW);

        len = UTF16toUTF8(currentDirW, -1, NULL, 0);
        parsedArgs->inPaths[0] = xcalloc(len, sizeof(char));

        UTF16toUTF8(currentDirW, -1, parsedArgs->inPaths[0], len);
        free(currentDirW);
#else
        if (!(parsedArgs->inPaths[0] = getcwd(NULL, 0))) {
            printErr("couldn't retrieve current working directory",
                     strerror(errno));
            return PARSE_STATE_CWD_ERROR;
        }
#endif
    }

    if (*parsedArgs->inFormats[0] == '\0') {
        printErr("no input format", "(NULL)");
        return PARSE_STATE_BAD_ARG;
    }

    if (!parsedArgs->outFormat) {
        printErr("no output format", "(NULL)");
        return PARSE_STATE_BAD_ARG;
    }

    if ((parsedArgs->options & (OPT_NEWFOLDER & OPT_CUSTOMFOLDERNAME))) {
        if ((strlen(parsedArgs->customFolder) >= NAME_MAX - 1)) {
            char *maxLen = _asprintf("%d bytes", NAME_MAX);
            printErr("custom folder name exceeds maximum allowed length", maxLen);
            free(maxLen);
        }

        return PARSE_STATE_LONG_ARG;
    }

    if (parsedArgs->options & OPT_NEWPATH) {
        if (parsedArgs->customPath == NULL || *parsedArgs->customPath == '\0') {
            printErr("empty custom path field", "usage: -outpath:[PATH]");

            return PARSE_STATE_BAD_ARG;
        }
    }

    for (int i = 0; parsedArgs->inFormats[i] != NULL; i++) {
        if (strcmp(parsedArgs->inFormats[i], parsedArgs->outFormat) == 0
            && !(parsedArgs->options & OPT_NEWFOLDER)
            && !(parsedArgs->options & OPT_NEWPATH)
            ) {
            printErr("can't use ffmpeg with identical input "
                     "and output formats",
                     "use '-outpath' or '-subfolder' "
                     "to save the files in a new directory");

            return PARSE_STATE_EXT_CONFLICT;
        }
    }

    return PARSE_STATE_OK;
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

static char *getAbsolutePath(const char *dir) {
#ifdef _WIN32
    wchar_t dirW[PATH_BUF];
    formatPathToWIN32(dir, dirW);

    wchar_t absDirW[PATH_BUF];
    GetFullPathNameW(dirW, PATH_BUF, absDirW, NULL);

    DWORD sz = UTF16toUTF8(absDirW, -1, NULL, 0);
    char *absDir = xcalloc(sz, sizeof(char));
    UTF16toUTF8(absDirW, -1, absDir, sz);

    return absDir;
#else
    /* TODO: do UNIX realpath() stuff here and test it
     * with very long paths (larger than PATH_MAX bytes)
     * NOTE: the malloc() version of realpath may not be
     * available in macos (in which case this will break) */
    return realpath(dir, NULL);
#endif
}
