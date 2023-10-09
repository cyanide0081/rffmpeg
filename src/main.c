#include <data.h>
#include <parse.h>
#include <search.h>
#include <convert.h>
#include <arena.h>
#include <help.h>

/* TODO:
 * - finish profiling the code's most important routines
 * - add multi-threading to the conversion procedure (and maybe searching too)
 */

Arena *globalArena = NULL; // for simplifying arena alloc calls

static void createTestProcess(void);
static void displayEndDialog(processInfo *procInfo);

int main(int argc, char *argv[]) {
    inputMode inputMode = argc > 1 ? ARGUMENTS : CONSOLE;

    GlobalArenaInit();

#ifdef _WIN32
    /* NOTE:
     * Windows Unicode console I/O prioritizing UTF-8:
     * 1. Set character translation mode for stdin to UTF-16LE
     * 2. Set console codepages to UTF-8
     * 3. Use wide chars for everything related to console
     *    input and WinAPI functions
     * 4. Convert everything else where needed to UTF-8 using
     *    WideCharToMultiByte() and use normal char functions
     *    for output to stdout or stderr */

    /* Set console input mode to Unicode (UTF-16LE) */
    _setmode(_fileno(stdin), _O_U16TEXT);

    UINT originalCP       = GetConsoleCP();
    UINT originalOutputCP = GetConsoleOutputCP();

    if (!IsValidCodePage(CP_UTF8) ||
        !SetConsoleCP(CP_UTF8) ||
        !SetConsoleOutputCP(CP_UTF8)
        ) {
        DWORD err = GetLastError();
        printWinErrMsg("couldn't set ANSI codepage to UTF-8", err);
        ExitProcess(err);
    }

    DWORD originalConsoleMode;
    enableVirtualTerminalProcessing(&originalConsoleMode);
    wchar_t originalConsoleWindowTitle[FILE_BUF];

    if (inputMode == CONSOLE) {
        size_t size = strlen(CONSOLE_WINDOW_TITLE) + 1;
        wchar_t *windowTitle = ArenaPush(globalArena, size * sizeof(wchar_t));

        UTF8toUTF16(CONSOLE_WINDOW_TITLE, -1, windowTitle, (int)size);
        GetConsoleTitleW(originalConsoleWindowTitle, FILE_BUF);
        SetConsoleTitleW(windowTitle);
    }

    int argcW = 0;
    wchar_t *cmdLine = GetCommandLineW();
    wchar_t **argvW = CommandLineToArgvW(cmdLine, &argcW);

    for (int i = 0; i < argcW; i++) {
        int size = UTF16toUTF8(argvW[i], -1, NULL, 0);
        argv[i] = GlobalArenaPush(size * sizeof(char));

        UTF16toUTF8(argvW[i], -1, argv[i], size);
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
        char **fileList = getFiles(parsedArgs);

        if (fileList) {
            size_t fileCount = 0;

            while (*(fileList + fileCount)) fileCount++;

            printf("%s > %sAbout to convert %s%d%s files. Continue? (Y/n):%s ",
                   COLOR_ACCENT, COLOR_DEFAULT,
                   COLOR_ACCENT, (int)fileCount, COLOR_DEFAULT,
                   COLOR_INPUT);

            int input = tolower(getchar());
            waitForNewLine();
            printf("%s\n", COLOR_DEFAULT);

            struct timespec startTime = {0};
            clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

            if (input == 'y') {
                exitCode = convertFiles((const char **)fileList,
                                       parsedArgs, &procInfo);
            } else {
                exitCode = EXIT_FAILURE;
            }

            struct timespec endTime = {0};
            clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);

            procInfo.executionTime =
                (double)(endTime.tv_sec - startTime.tv_sec) +
                (endTime.tv_nsec - startTime.tv_nsec) / 1e9F;

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

        waitForNewLine();
        printf("\n");

#ifdef _WIN32
        SetConsoleTitleW(originalConsoleWindowTitle);
#endif
    }

#ifdef _WIN32
    SetConsoleCP(originalCP);
    SetConsoleOutputCP(originalOutputCP);
    restoreConsoleMode(originalConsoleMode);
#endif

    GlobalArenaRelease();

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
            char status[FMT_BUF];
            snprintf(status, FMT_BUF, "exit status: %d", exitStatus);
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

        printf(" %sDONE!%s\n\n", COLOR_ACCENT, COLOR_DEFAULT);

        printf(" %sProcessed files: %s%lu%s\n", COLOR_DEFAULT,
               COLOR_ACCENT, (unsigned long)procInfo->convertedFiles,
               COLOR_DEFAULT);
        printf(" %sDeleted files:   %s%lu%s\n", COLOR_DEFAULT,
               COLOR_ACCENT, (unsigned long)procInfo->deletedFiles,
               COLOR_DEFAULT);
        printf(" %sElapsed time:    %s%02lu:%02lu:%05.2lf%s\n\n",
               COLOR_DEFAULT, COLOR_ACCENT,
               (unsigned long)execTime.hours,
               (unsigned long)execTime.minutes,
               execTime.seconds,
               COLOR_DEFAULT);
    }
}

/* code profiling wrapper functions */
#ifdef  INSTRUMENTATION
#define __USE_GNU
#include <dlfcn.h>

static struct timespec funcStartTime = {0};
static struct timespec funcEndTime   = {0};

/* overriding clang's instrumentation functions for profiling */
void __attribute__((__no_instrument_function__))
    __cyg_profile_func_enter(void *thisFunc, void *callSite) {
    (void)callSite;

    clock_gettime(CLOCK_MONOTONIC_RAW, &funcStartTime);

    Dl_info info;
    dladdr(thisFunc, &info);

    if (info.dli_sname)
        printf(" calling %s():", info.dli_sname);
}

void __attribute__((__no_instrument_function__))
    __cyg_profile_func_exit(void *thisFunc, void *callSite) {
    (void)callSite;

    clock_gettime(CLOCK_MONOTONIC_RAW, &funcEndTime);

    double time =
        (double)(funcEndTime.tv_sec - funcStartTime.tv_sec) +
        (funcEndTime.tv_nsec - funcStartTime.tv_nsec) / 1e9F;

    Dl_info info;
    dladdr(thisFunc, &info);

    if (info.dli_sname)
        printf(" exiting %s() [elapsed time: %.3lfμs]\n\n",
               info.dli_sname, time * 1e6F);
}

#endif
