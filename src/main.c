#include "../lib/libs.h"
#include "../lib/headers.h"
#include <stdlib.h>

/* TODO:
 * implement argument parsing interface (expectToken() and stuff)
 * implement --version command
 */

int main(int argc, char *argv[]) {
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

#ifdef _WIN32
    /*
     * Windows Unicode I/O prioritizing UTF-8:
     *
     * 1. Set character translation mode for stdin to UTF-16LE
     * 2. Set console codepages to UTF-8
     * 3. Use wide chars for everything related to console
     *    input and WinAPI functions
     * 4. Convert everything else where needed to UTF-8 using
     *    WideCharToMultiByte() and use normal char functions
     *    for output to stdout or stderr
     */

    #define UNICODE
    #define _UNICODE

    /* Setup Unicode (UTF-16LE) console Input for Windows */
    _setmode(_fileno(stdin), _O_U16TEXT);

    UINT originalCP       = GetConsoleCP();
    UINT originalOutputCP = GetConsoleOutputCP();

    /* Set all code pages to UTF-8 */
    if (!IsValidCodePage(CP_UTF8) |
        !SetConsoleCP(CP_UTF8) |
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
    wchar_t originalConsoleWindowTitle[FILE_BUFFER];

    if (inputMode == CONSOLE) {
        size_t size = strlen(CONSOLE_WINDOW_TITLE) + 1;
        wchar_t *windowTitle = xcalloc(size, sizeof(wchar_t));

        UTF8toUTF16(CONSOLE_WINDOW_TITLE, -1, windowTitle, (int)size);
        GetConsoleTitleW(originalConsoleWindowTitle, FILE_BUFFER);
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

    processInfo processInfo = { 0 };

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
        displayHelp();
    } else if ((exitCode = handleArgErrors(parsedArgs)) == EXIT_SUCCESS) {
        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        exitCode = searchDirs(parsedArgs, &processInfo);

        clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);

        processInfo.executionTime =
            (double)(endTime.tv_sec - startTime.tv_sec) +
            (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

        if (exitCode == EXIT_SUCCESS)
            displayEndDialog(&processInfo);
    }

    if (inputMode == CONSOLE) {
        printf(" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);
#ifdef _WIN32
        getwchar();
#else
        getchar();
#endif
        printf("\n");
    }

#ifdef _WIN32
    if (inputMode == CONSOLE)
        SetConsoleTitleW(originalConsoleWindowTitle);

    SetConsoleCP(originalCP);
    SetConsoleOutputCP(originalOutputCP);

    restoreConsoleMode(originalConsoleMode);

    for (int i = 0; i < argc; i++)
        free(argv[i]);
#endif  /* _WIN32 */

    destroyArguments(parsedArgs);

    return exitCode;
}
