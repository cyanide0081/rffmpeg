#include "../include/libs.h"
#include "../include/headers.h"

/* NOTE: this code adheres to a not-so-strict 100-column margin */

/* TODO: 
 * - restructure the whole code to only make use of portable I/O
 *   functions and become ready for compiling for Unix-like OS's
 * 
 * - replace the for-loop-enclosed call to searchDirectory() with
 *   a wrapper function
 */

int wmain(int argc, char16_t *argv[]) {
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT); // Setup Unicode (UTF-16LE) console I/O for Windows
    #endif
    
    processInfo processInformation = { 0 };
    inputMode inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    /* Enable virtual terminal sequences for colored console output */
    DWORD originalConsoleMode;
    char16_t originalConsoleWindowTitle[PATH_BUFFER];
    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATH_BUFFER);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf_s(u"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);
    
    int exitCode = createTestProcess();

    arguments *parsedArguments = initializeArguments();

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, parsedArguments);
    } else {
        parseConsoleInput(parsedArguments);
    }

    if (parsedArguments->options & OPT_DISPLAYHELP && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleArgumentErrors(parsedArguments)) == EXIT_SUCCESS) {
        clock_t startTime = clock();

        for (int i = 0; i < parsedArguments->inPathsCount; i++)
            exitCode = searchDirectory(parsedArguments->inPaths[i], parsedArguments,
             &processInformation);

        clock_t endTime = clock();

        if (exitCode == EXIT_SUCCESS) {
            processInformation.executionTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            
            displayEndDialog(&processInformation);
        }
    }

    if (inputMode == CONSOLE) {
        wprintf_s(u" %ls(Press any key to exit) %ls", CHARCOLOR_WHITE, COLOR_DEFAULT);
        getwchar();

        wprintf_s(u"\n");
        SetConsoleTitleW(originalConsoleWindowTitle);
    }

    destroyArguments(parsedArguments);
    restoreConsoleMode(originalConsoleMode);

    return exitCode;
}