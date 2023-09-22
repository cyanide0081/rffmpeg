#include <libs.h>
#include <data.h>
#include <parse.h>
#include <search.h>
#include <convert.h>
#include <help.h>

/* TODO:
 * 1. create macro for windows error message formatting (stupidly long proc)
 * 2. detect if only directories were passed as arguments and if so take program
 *    flow to the second argument of the console parsing section
 * 2. add multi-threading to the conversion procedure (and maybe searching too)
 * 3. implement --version command (maybe)
 *
 * FIXME:
 * 1. resolve non-absolute pathnames by feeding them to realpath()
 *    before passing them to the main file-searching procedure
 *    (or implement your own, since the available one kinda sucks) */

static int handleArgErrors(arguments *args);
static void createTestProcess(void);
static void displayEndDialog(processInfo *procInfo);

int main(int argc, char *argv[]) {
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

#ifdef _WIN32
    /* NOTE: Windows Unicode console I/O prioritizing UTF-8:
     * 1. Set character translation mode for stdin to UTF-16LE
     * 2. Set console codepages to UTF-8
     * 3. Use wide chars for everything related to console
     *    input and WinAPI functions
     * 4. Convert everything else where needed to UTF-8 using
     *    WideCharToMultiByte() and use normal char functions
     *    for output to stdout or stderr */

    /* Setup Unicode (UTF-16LE) console Input for Windows */
    _setmode(_fileno(stdin), _O_U16TEXT);

    UINT originalCP       = GetConsoleCP();
    UINT originalOutputCP = GetConsoleOutputCP();

    /* Set all code pages to UTF-8 */
    if (!IsValidCodePage(CP_UTF8) ||
        !SetConsoleCP(CP_UTF8) ||
        !SetConsoleOutputCP(CP_UTF8)
        ) {
        DWORD err = GetLastError();
        wchar_t *errMsgW = NULL;
        int sizeW = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&errMsgW,
            0,
            NULL
        );

        int size = UTF16toUTF8(errMsgW, (int)sizeW, NULL, 0);
        char *errMsg = xcalloc(size, sizeof(char));
        UTF16toUTF8(errMsgW, sizeW, errMsg, size);
        trimSpaces(errMsg);

        printErr("couldn't set ANSI codepage to UTF-8", errMsg);
        LocalFree(errMsgW);
        free(errMsg);
        exit(err);
    }

    /* Enable virtual terminal sequences for colored console output */
    DWORD originalConsoleMode;
    enableVirtualTerminalProcessing(&originalConsoleMode);
    wchar_t originalConsoleWindowTitle[FILE_BUF];

    if (inputMode == CONSOLE) {
        size_t size = strlen(CONSOLE_WINDOW_TITLE) + 1;
        wchar_t *windowTitle = xcalloc(size, sizeof(wchar_t));

        UTF8toUTF16(CONSOLE_WINDOW_TITLE, -1, windowTitle, (int)size);
        GetConsoleTitleW(originalConsoleWindowTitle, FILE_BUF);
        SetConsoleTitleW(windowTitle);
        free(windowTitle);
    }

    int argcW = 0;
    wchar_t *cmdLine = GetCommandLineW();
    wchar_t **argvW = CommandLineToArgvW(cmdLine, &argcW);

    for (int i = 0; i < argcW; i++) {
        size_t size = UTF16toUTF8(argvW[i], -1, NULL, 0);
        argv[i] = xcalloc(size, sizeof(char));

        UTF16toUTF8(argvW[i], -1, argv[i], (int)size);
    }

    LocalFree(argvW);
#endif  /* _WIN32 */

    processInfo procInfo = {0};

    printf("%s%s%s\n\n", CHARCOLOR_RED, FULL_PROGRAM_TITLE, COLOR_DEFAULT);

    createTestProcess();

    arguments *parsedArgs = initializeArguments();

    if (inputMode == ARGUMENTS) {
        parseArgs(argc, argv, parsedArgs);
    } else {
        parseConsoleInput(parsedArgs);
    }

    int exitCode = EXIT_SUCCESS;

    if (parsedArgs->options & OPT_DISPLAYHELP && inputMode == ARGUMENTS) {
        printf(HELP_PAGE);
    } else if ((exitCode = handleArgErrors(parsedArgs)) == EXIT_SUCCESS) {
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        char **fileList = getFiles(parsedArgs);

        if (fileList) {
            size_t fileCount = 0;

            while (*(fileList + fileCount))
                fileCount++;

            printf("%s > %sAbout to convert %s%d%s files. Continue? (Y/n):%s ",
                   CHARCOLOR_RED, CHARCOLOR_WHITE,
                   CHARCOLOR_RED, (int)fileCount, CHARCOLOR_WHITE,
                   CHARCOLOR_WHITE_BOLD);

            int input = tolower(getchar());
            printf("\n");

            if (input == 'y') {
                exitCode = convertFiles((const char **)fileList,
                                       parsedArgs, &procInfo);
            } else {
                exitCode = EXIT_FAILURE;
            }

            for (int i = 0; fileList[i]; i++)
                free(fileList[i]);

            free(fileList);

            clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);

            procInfo.executionTime =
                (double)(endTime.tv_sec - startTime.tv_sec) +
                (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

            if (exitCode != EXIT_FAILURE)
                displayEndDialog(&procInfo);
        } else {
            printErr("found no matching files", "aborting");
        }
    }

    if (inputMode == CONSOLE) {
        printf(" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getchar();
        printf("\n");

#ifdef _WIN32
        SetConsoleTitleW(originalConsoleWindowTitle);
#endif
    }

#ifdef _WIN32
    SetConsoleCP(originalCP);
    SetConsoleOutputCP(originalOutputCP);
    restoreConsoleMode(originalConsoleMode);

    for (int i = 0; i < argc; i++)
        free(argv[i]);
#endif

    destroyArguments(parsedArgs);
    return exitCode;
}

/* Handles edge cases regarding arguments as well as errors */
static int handleArgErrors(arguments *args) {
    int code = EXIT_SUCCESS;

    /* Set current working directory as input path if none is provided */
    if (args->inPaths[0] == NULL) {
#ifdef _WIN32
        int len = GetCurrentDirectoryW(0, NULL);
        wchar_t *currentDirW = xcalloc(len, sizeof(wchar_t));
        GetCurrentDirectoryW((DWORD)len, currentDirW);

        len = UTF16toUTF8(currentDirW, -1, NULL, 0);
        args->inPaths[0] = xcalloc(len, sizeof(char));

        UTF16toUTF8(currentDirW, -1, args->inPaths[0], len);
        free(currentDirW);
#else
        if (!(args->inPaths[0] = getcwd(NULL, 0))) {
            printErr("couldn't retrieve current working directory",
                     strerror(errno));
            exit(errno);
        }
#endif
    }

    if (args->ffOptions == NULL)
        args->ffOptions = strdup("");

    if (args->inFormats[0] == NULL || *args->inFormats[0] == '\0') {
        printErr("no input format", "NULL");
        /* printf("%s run with --help%s for more information%s\n\n", */
        /*        CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD, COLOR_DEFAULT); */
        code = EXIT_FAILURE;
    }

    if (args->outFormat == NULL || *args->outFormat == '\0') {
        printErr("no output format", "NULL");
        /* printf("%s run with --help%s for more information%s\n\n", */
        /*        CHARCOLOR_WHITE, CHARCOLOR_WHITE_BOLD, COLOR_DEFAULT); */
        code = EXIT_FAILURE;
    }

    if ((args->options & OPT_NEWFOLDER)
        && (strlen(args->customFolderName) >= NAME_MAX - 1)
        ) {
        char *maxLen = _asprintf("%d bytes", NAME_MAX);
        printErr("custom folder name exceeds maximum allowed length", maxLen);
        free(maxLen);

        code = EXIT_FAILURE;
    }

    if (args->options & OPT_NEWPATH) {
        if (args->customPathName == NULL) {
            printErr("empty custom pathname field", "usage: --newpath=name");

            code = EXIT_FAILURE;
        }

        /* TODO: prompt the user to choose whether they want
           to remove windows's default pathname limit 8) */
#ifdef _WIN32
        if (strlen(args->customPathName) >= MAX_PATH) {
            char *maxLength = _asprintf("%d bytes", MAX_PATH);

            printErr("custom path name exceeds maximum allowed length",
                     maxLength);
            free(maxLength);

            code = EXIT_FAILURE;
        }
#endif
    }

    for (int i = 0; args->inFormats[i] != NULL; i++) {
        if (strcmp(args->inFormats[i], args->outFormat) == 0
            && !(args->options & OPT_NEWFOLDER)
            && !(args->options & OPT_NEWPATH)
            ) {
            printErr("can't use ffmpeg with identical input "
                     "and output formats",
                     "use '-newpath' or '-subfolder' "
                     "to save the files in a new directory");

            code = EXIT_FAILURE;
            break;
        }
    }

    return code;
}

static void createTestProcess(void) {
#ifdef _WIN32
    STARTUPINFOW ffmpegStartupInfo = {0};
    PROCESS_INFORMATION ffmpegProcessInfo;
    wchar_t ffmpegProcessCall[] = u"ffmpeg -loglevel quiet";

    if (CreateProcessW(NULL, ffmpegProcessCall, NULL, NULL, FALSE, 0,
                       NULL, NULL, &ffmpegStartupInfo, &ffmpegProcessInfo)) {
        WaitForSingleObject(ffmpegProcessInfo.hProcess, INFINITE);
        CloseHandle(ffmpegProcessInfo.hProcess);
        CloseHandle(ffmpegProcessInfo.hThread);

        return;
    }

    DWORD err = GetLastError();
    wchar_t *errMsgW = NULL;
    int sizeW = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                               FORMAT_MESSAGE_FROM_SYSTEM |
                               FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL,
                               err,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPWSTR)&errMsgW,
                               0,
                               NULL);

    int size = UTF16toUTF8(errMsgW, (int)sizeW, NULL, 0);
    char *errMsg = xcalloc(size, sizeof(char));
    UTF16toUTF8(errMsgW, sizeW, errMsg, size);

    printErr("couldn't start ffmpeg", errMsg);
    LocalFree(errMsgW);
    free(errMsg);
    exit(err);
#else
    pid_t processID = fork();

    if (processID == 0) {
        execlp("ffmpeg", "ffmpeg", "-loglevel", "quiet", (char*)NULL);
        exit(errno);
    } else {
        int status;
        waitpid(processID, &status, 0);

        int exitStatus = 0;

        if (!WIFEXITED(status))
            exitStatus = WEXITSTATUS(status);

        /* Status 1 means the call succeeded and ffmpeg
           returned an error, and 2 means it wasn't found
           TODO: handle more exit codes down here! */
        if (exitStatus != 0) {
            char status[FILE_BUF];
            snprintf(status, FILE_BUF - 1, "exit status: %d", exitStatus);
            printErr("couldn't call ffmpeg", status);
            exit(EXIT_FAILURE);
        }
    }

    return;
#endif
}

static void displayEndDialog(processInfo *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printErr("unable to convert files",
                 "double-check your ffmpeg parameters");
    } else {
        fmtTime executionTime = formatTime(processInformation->executionTime);

        printf(" %sDONE!%s\n", CHARCOLOR_RED, COLOR_DEFAULT);
        printf("\n");
        printf(" %sProcessed files: %s%" PRIu64 "%s\n", CHARCOLOR_WHITE,
               CHARCOLOR_RED, (uint64_t)processInformation->convertedFiles,
               COLOR_DEFAULT);
        printf(" %sDeleted files:   %s%" PRIu64 "%s\n", CHARCOLOR_WHITE,
               CHARCOLOR_RED, (uint64_t)processInformation->deletedFiles,
               COLOR_DEFAULT);
        printf(" %sElapsed time:    %s%02" PRIu64 ":%02" PRIu64 ":%05.2lf%s\n",
               CHARCOLOR_WHITE, CHARCOLOR_RED,
               executionTime.hours, executionTime.minutes, executionTime.seconds,
               COLOR_DEFAULT);
        printf("\n");
    }
}
