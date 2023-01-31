#include "../include/libs.h"

int wmain(int argc, const char16_t *argv[]) {
    char16_t *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };

    DWORD originalConsoleMode;
    char16_t originalConsoleWindowTitle[PATHBUF];
 
    errorCode_t exitCode = ERROR_NONE;
    processInfo_t processInformation = { 0 };
    inputMode_t inputMode = argc == 1 ? CONSOLE : ARGUMENTS;

    _setmode(_fileno(stdout), _O_U16TEXT); // Setup Unicode (UTF-16LE) console I/O

    allocateArgumentBuffers(arguments);    
    enableVirtualTerminalProcessing(&originalConsoleMode);

    if (inputMode == CONSOLE) {
        GetConsoleTitleW(originalConsoleWindowTitle, PATHBUF);
        SetConsoleTitleW(consoleWindowTitle);
    }

    wprintf_s(u"%ls%ls%ls\n\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);

    if (inputMode == ARGUMENTS) {
        parseArguments(argc, argv, arguments, options, true, true);
    } else {
        parseArgumentsFromTerminal(arguments, options);
    }

    if (options[OPT_DISPLAYHELP] == true && inputMode == ARGUMENTS) {
        displayHelp();
    } else if ((exitCode = handleErrors(arguments)) == ERROR_NONE) {
        clock_t startTime = clock();
        exitCode = searchDirectory(NULL, arguments, options, &processInformation);
        clock_t endTime = clock();

        if (exitCode == ERROR_NONE) {
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

    freeArgumentBuffers(arguments);
    restoreConsoleMode(originalConsoleMode);

    return (int)exitCode;
}