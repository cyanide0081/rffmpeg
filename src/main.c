#include "../include/libs.h"
#include "../include/headers.h"

/* TODO:
 * implement argument parsing interface (expectToken() and stuff)
 * implement --version command
 * compartmentalize the long os-checking #ifdefs into their own functions
 */

int main(int argc, char *argv[]) {
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

#ifdef _WIN32

#define UNICODE
#define _UNICODE

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

    /* Setup Unicode (UTF-16LE) console Input for Windows */
    _setmode(_fileno(stdin), _O_U16TEXT);

    /* Set all code pages to UTF-8 */
    if (!IsValidCodePage(CP_UTF8))
      return GetLastError();

    UINT originalCP       = GetConsoleCP();
    UINT originalOutputCP = GetConsoleOutputCP();

    if (!SetConsoleCP(CP_UTF8))
      return GetLastError();

    if (!SetConsoleOutputCP(CP_UTF8))
      return GetLastError();

    /* Enable virtual terminal sequences for colored console output */
    DWORD originalConsoleMode;
    enableVirtualTerminalProcessing(&originalConsoleMode);
    wchar_t originalConsoleWindowTitle[FILE_BUFFER];

    if (inputMode == CONSOLE) {
      size_t size = strlen(CONSOLE_WINDOW_TITLE) + 1;
      wchar_t *windowTitle = xcalloc(size, sizeof(wchar_t));

      utf8toutf16(CONSOLE_WINDOW_TITLE, -1, windowTitle, (int)size);
      GetConsoleTitleW(originalConsoleWindowTitle, FILE_BUFFER);
      SetConsoleTitleW(windowTitle);
      free(windowTitle);
    }

    int argcW = 0;
    wchar_t *cmdLine = GetCommandLineW();
    wchar_t **argvW = CommandLineToArgvW(cmdLine, &argcW);

    for (int i = 0; i < argcW; i++) {
      size_t size = utf16toutf8(argvW[i], -1, NULL, 0);
      argv[i] = xcalloc(size, sizeof(char));

      utf16toutf8(argvW[i], -1, argv[i], (int)size);
    }

    LocalFree(argvW);
#endif  /* _WIN32 */

    processInfo processInfo = { 0 };

    printf("%s%s%s\n\n", CHARCOLOR_RED, FULL_PROGRAM_TITLE, COLOR_DEFAULT);

    int exitCode = createTestProcess();

    if (exitCode == EXIT_FAILURE) {
        char errormsg[FILE_BUFFER] = "";
        strerror_s(errormsg, NAME_MAX, errno);

        printerr("couldn't find FFmpeg", errormsg);
        exit(EXIT_FAILURE);
    }

    arguments *parsedArgs = initializeArguments();

    if (inputMode == ARGUMENTS) {
        parseArgs(argc, argv, parsedArgs);
    } else {
        parseConsoleInput(parsedArgs);
    }

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
#endif  /* _WIN32 */

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
