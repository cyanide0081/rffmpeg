#include "../include/libs.h"
#include "../include/headers.h"

int main(int argc, char *argv[]) {
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    #ifdef _WIN32
       /* 
        * Windows Unicode I/O prioritizing UTF-8:
        * 
        * 1. Set character translation mode for stdin to UTF-16LE  
        * 2. Set console codepages to UTF-8
        * 3. Use wide chars for everything related to console input and WinAPI functions
        * 4. Convert everything else where needed to UTF-8 using WideCharToMultiByte()
        *    and use normal char functions for output to stdout or stderr
        */

        #define UNICODE
        #define _UNICODE

        /* Setup Unicode (UTF-16LE) console Input for Windows */
        setmode(_fileno(stdin), _O_U16TEXT);

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
            GetConsoleTitleW(originalConsoleWindowTitle, FILE_BUFFER);
            SetConsoleTitleW((wchar_t*)CONSOLE_WINDOW_TITLE);
        }

        int argcW = 0;
        wchar_t *cmdLine = GetCommandLineW();
        wchar_t **argvW = CommandLineToArgvW(cmdLine, &argcW);

        for (int i = 0; i < argcW; i++) {
            size_t size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);

            argv[i] = xcalloc(size, sizeof(char));

            WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], size, NULL, NULL);
        }

        LocalFree(argvW);
    #endif

    processInfo processInfo = { 0 };

    printf("%s%s%s\n\n", CHARCOLOR_RED, FULL_PROGRAM_TITLE, COLOR_DEFAULT);
    
    int exitCode = createTestProcess();

    if (exitCode == EXIT_FAILURE) {  
        printError("couldn't find FFmpeg", strerror(errno));
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
        #ifdef _WIN32
            #define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
        #endif

        struct timespec startTime, endTime;
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);

        exitCode = searchDirs(parsedArgs, &processInfo);

        clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
        
        processInfo.executionTime = 
         (double)(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

        if (exitCode == EXIT_SUCCESS)
            displayEndDialog(&processInfo);
    }

    if (inputMode == CONSOLE) {
        printf(" %s(Press any key to exit) %s", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getchar();

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
    #endif

    destroyArguments(parsedArgs);

    return exitCode;
}