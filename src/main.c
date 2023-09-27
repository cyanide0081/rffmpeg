#include <data.h>
#include <parse.h>
#include <search.h>
#include <convert.h>
#include <help.h>

/* TODO:
 * - add multi-threading to the conversion procedure (and maybe searching too)
 * - implement --version command (maybe)
 *
 * FIXME:
 * - resolve non-absolute pathnames by feeding them to realpath()
 *   before passing them to the main file-searching procedure
     (or implement your own, since the available one kinda sucks) */

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
        printWinErrMsg("couldn't set ANSI codepage to UTF-8", err);
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

    printf("%s%s%s\n\n", COLOR_ACCENT,
           "RFFMPEG " PROGRAM_VERSION " (月光)", COLOR_DEFAULT);

    createTestProcess();

    arguments *parsedArgs = allocArguments();

    int exitCode = EXIT_SUCCESS;
    int state = PARSE_STATE_OK;

    if (inputMode == ARGUMENTS) {
        state = parseArgs(argc, argv, parsedArgs);
    } else {
        state = parseConsoleInput(parsedArgs);
    }

    if (parsedArgs->options & OPT_DISPLAYHELP && inputMode == ARGUMENTS) {
        printf(HELP_PAGE);
    } else if (state == PARSE_STATE_OK) {
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        char **fileList = getFiles(parsedArgs);

        if (fileList) {
            size_t fileCount = 0;

            while (*(fileList + fileCount))
                fileCount++;

            printf("%s > %sAbout to convert %s%d%s files. Continue? (Y/n):%s ",
                   COLOR_ACCENT, COLOR_DEFAULT,
                   COLOR_ACCENT, (int)fileCount, COLOR_DEFAULT,
                   COLOR_INPUT);

            int input = tolower(getchar());
            printf("%s\n", COLOR_DEFAULT);

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
            printf(" found no matching files (%saborting%s)\n\n",
                   COLOR_ACCENT, COLOR_DEFAULT);
        }
    }

    if (inputMode == CONSOLE) {
        printf(" %s(Press %sENTER%s to exit) ",
               COLOR_DEFAULT, COLOR_INPUT, COLOR_DEFAULT);

        _waitForNewLine();
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

    freeArguments(parsedArgs);
    return exitCode;
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
    printWinErrMsg("couldn't start ffmpeg", err);
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

static void displayEndDialog(processInfo *procInfo) {
    if (procInfo->convertedFiles == 0) {
        printErr("unable to convert files",
                 "double-check your ffmpeg parameters");
    } else {
        fmtTime execTime = formatTime(procInfo->executionTime);

        printf(" %sDONE!%s\n", COLOR_ACCENT, COLOR_DEFAULT);
        printf("\n");
        printf(" %sProcessed files: %s%" PRIu64 "%s\n", COLOR_DEFAULT,
               COLOR_ACCENT, (uint64_t)procInfo->convertedFiles,
               COLOR_DEFAULT);
        printf(" %sDeleted files:   %s%" PRIu64 "%s\n", COLOR_DEFAULT,
               COLOR_ACCENT, (uint64_t)procInfo->deletedFiles,
               COLOR_DEFAULT);
        printf(" %sElapsed time:    %s%02" PRIu64 ":%02" PRIu64 ":%05.2lf%s\n",
               COLOR_DEFAULT, COLOR_ACCENT,
               execTime.hours, execTime.minutes, execTime.seconds,
               COLOR_DEFAULT);
        printf("\n");
    }
}
