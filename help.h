#ifndef HEADER_HELP
#define HEADER_HELP

#include "libs.h"

static void displayHelp(void) {
    wprintf_s(L"%ls\n", fullTitle);
    wprintf_s(L" Use me to automate an ffmpeg conversion throughout a whole directory\n");
    wprintf_s(L"\n");
    wprintf_s(L" Arguments:\n");
    wprintf_s(L"   -i <path>          the path you want to search through\n");
    wprintf_s(L"   -f <format(s)>     the file format(s) you want to find and convert (e.g.: mp4,mov)\n");
    wprintf_s(L"   -p <parameters>    the ffmpeg conversion options\n");
    wprintf_s(L"   -o <output>        the output container (e.g.: mkv)\n");
    wprintf_s(L"\n");
    wprintf_s(L" Optional modes:\n");
    wprintf_s(L"   /r                 disable recursive subfolder search\n");
    // wprintf_s(L"   /n                 store the converted files in a new folder     (not implemented)\n");
    // wprintf_s(L"   /d                 delete the original files after conversion    (not implemented)\n");
    wprintf_s(L"   /y                 auto-overwrite existing output files (default behaviour appends an index)\n");
    wprintf_s(L"\n");
    wprintf_s(L" Example usage: rffmpeg -i C:\\Users\\Music -f wav,aif -p \"-c:a libmp3lame\" -o mp3\n");
}

#endif