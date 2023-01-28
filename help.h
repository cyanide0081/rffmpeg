#ifndef HEADER_HELP
#define HEADER_HELP

#include "libs.h"

static void displayHelp(void) {
    printf_s(u8" Use me to automate an ffmpeg conversion throughout a whole directory recursively (or not)\n");
    printf_s(u8"\n");
    printf_s(u8" Arguments:\n");
    printf_s(u8"    %-20lsthe directory containing your input files (defaults to current working path if left empty)\n", u8"-path <path>");
    printf_s(u8"    %-20lsthe file format(s) you want to find and convert (e.g.: mp4,mov)\n", u8"-fmt <format(s)>");
    printf_s(u8"    %-20lsthe ffmpeg conversion-related options (e.g.: -c:v libx264 -c:a aac -f matroska)\n", u8"-opts <ffmpeg options>");
    printf_s(u8"    %-20lsthe output extension (e.g.: mkv)\n", u8"-ext <output>");
    printf_s(u8"\n");
    printf_s(u8" Additional options:\n");
    printf_s(u8"    %-20lsstore converted files in a new folder - you can choose a folder name with '--newfolder=name' (without whitespaces)\n", OPT_MAKENEWFOLDER_STRING);
    printf_s(u8"    %-20lsauto-overwrite existing output files (default behaviour appends an index)\n", OPT_FORCEOVERWRITE_STRING);
    printf_s(u8"    %-20lsdelete original files after conversion \n", OPT_DELETEOLDFILES_STRING);
    printf_s(u8"    %-20lsdisable recursive subfolder search\n", OPT_DISABLERECURSION_STRING);
    printf_s(u8"\n");
    printf_s(u8" Example usage: rffmpeg -path C:\\Users\\Music -fmt wav,aif -opts \"-c:a libmp3lame\" -ext mp3\n");
    printf_s(u8"\n");
}

#endif