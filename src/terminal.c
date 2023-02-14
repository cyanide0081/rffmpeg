#include "../include/terminal.h"

void printError(const char16_t  *msg) {
    fwprintf_s(stderr, u"%lsERROR: %ls%ls%ls\n\n",
     CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

int restoreConsoleMode(DWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (SetConsoleMode(handleToStdOut, originalConsoleMode == false))
        return GetLastError();
    
    return NO_ERROR;
}

int enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;

    if (GetConsoleMode(handleToStdOut, &consoleMode) == false)
        return GetLastError();

    *originalConsoleMode = consoleMode;
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (SetConsoleMode(handleToStdOut, consoleMode) == false)
        return GetLastError();

    return NO_ERROR;
}

void displayEndDialog(processInfo *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printError(u"couldn't convert input files (undefined)");
    } else {
        formattedTime executionTime = formatTime(processInformation->executionTime);

        wprintf_s(u" %lsDONE!%ls\n", CHARCOLOR_RED, COLOR_DEFAULT);
        wprintf_s(u"\n");
        wprintf_s(u" %lsProcessed files: %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, 
         processInformation->convertedFiles, COLOR_DEFAULT);
        wprintf_s(u" %lsDeleted files:   %ls%llu%ls\n", CHARCOLOR_WHITE, CHARCOLOR_RED, 
         processInformation->deletedFiles, COLOR_DEFAULT);
        wprintf_s(u" %lsElapsed time:    %ls%02llu:%02llu:%05.2lf%ls\n",
         CHARCOLOR_WHITE, CHARCOLOR_RED,
         executionTime.hours, executionTime.minutes, executionTime.seconds,
         COLOR_DEFAULT);
        wprintf_s(u"\n");
    }
 }