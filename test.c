#include <stdio.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

int main(int argc, char *argv[]) {
    /* 
     * (Seems like this works :DDD)
     * 
     * Windows Unicode I/O prioritizing UTF-8:
     * 
     * 1. Set character translation mode for stdin to UTF-16LE  
     * 2. Set console codepages to UTF-8
     * 3. Use wide chars for everything related to console input and WinAPI functions
     * 4. Convert everything else where needed to UTF-8 using WideCharToMultiByte() and use normal
     *    char functions for output to stdout or stderr
     */

    #define UNICODE
    #define _UNICODE

    if (!IsValidCodePage(CP_UTF8))
        return GetLastError();

    if (!SetConsoleCP(CP_UTF8))
        return GetLastError();

    if (!SetConsoleOutputCP(CP_UTF8))
        return GetLastError();

    /* Setup Unicode (UTF-16LE) console I/O for Windows */
    setmode(_fileno(stdin), _O_U16TEXT);

    wchar_t input[MAX_PATH];
    char inputNarrow[MAX_PATH];

    fgetws(input, MAX_PATH, stdin);

    WideCharToMultiByte(CP_UTF8, 0, input, -1, inputNarrow, PATH_MAX, NULL, FALSE);

    printf("input: %s\n", inputNarrow);
}