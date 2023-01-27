#ifndef HEADER_HELP
#define HEADER_HELP

#include "libs.h"

static void displayHelp(void) {
    wprintf_s(L" Use me to automate an ffmpeg conversion throughout a whole directory recursively (or not)\n");
    wprintf_s(L"\n");
    wprintf_s(L" Arguments:\n");
    wprintf_s(L"    %-20lsthe directory containing your input files (defaults to current working path if left empty)\n", L"-path <path>");
    wprintf_s(L"    %-20lsthe file format(s) you want to find and convert (e.g.: mp4,mov)\n", L"-fmt <format(s)>");
    wprintf_s(L"    %-20lsthe ffmpeg conversion-related options (e.g.: -c:v libx264 -c:a aac -f matroska)\n", L"-opts <ffmpeg options>");
    wprintf_s(L"    %-20lsthe output extension (e.g.: mkv)\n", L"-ext <output>");
    wprintf_s(L"\n");
    wprintf_s(L" Additional options:\n");
    wprintf_s(L"    %-20lsstore converted files in a new folder (you can choose a folder name with --newfolder=name)\n", OPT_MAKENEWFOLDER_STRING);
    wprintf_s(L"    %-20lsauto-overwrite existing output files (default behaviour appends an index)\n", OPT_FORCEOVERWRITE_STRING);
    wprintf_s(L"    %-20lsdelete original files after conversion \n", OPT_DELETEOLDFILES_STRING);
    wprintf_s(L"    %-20lsdisable recursive subfolder search\n", OPT_DISABLERECURSION_STRING);
    wprintf_s(L"\n");
    wprintf_s(L" Example usage: rffmpeg -path C:\\Users\\Music -fmt wav,aif -opts \"-c:a libmp3lame\" -ext mp3\n");
    wprintf_s(L"\n");
}

#endif