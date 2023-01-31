#ifndef HEADER_HELP
#define HEADER_HELP

#include "../libs.h"

static void displayHelp(void) {
    printf_s(" Use me to automate an ffmpeg conversion inside a directory and its subfolders\n");
    printf_s("\n");
    printf_s(" Arguments:\n");
    printf_s("\t%-25sthe directory containing your input files (defaults to current working path if left empty)\n", "-path <path>");
    printf_s("\t%-25sthe file format(s) you want to find and convert (e.g.: mp4,mov)\n", "-fmt <format(s)>");
    printf_s("\t%-25sthe ffmpeg conversion-related options (e.g.: -c:v libx264 -c:a aac -f matroska)\n", "-opts <options>");
    printf_s("\t%-25sthe output extension (e.g.: mkv)\n", "-ext <extension>");
    printf_s("\n");
    printf_s(" Additional flags:\n");
    printf_s("\t%-25sstore converted files in a new folder - you can choose a folder name with '--newfolder=foldername'\n", OPT_MAKENEWFOLDER_STRING);
    printf_s("\t%-25sauto-overwrite existing output files (default behaviour appends an index)\n", OPT_FORCEOVERWRITE_STRING);
    printf_s("\t%-25sdelete original files after conversion (PERMANENT) \n", OPT_DELETEOLDFILES_STRING);
    printf_s("\t%-25sdisable recursive subfolder search\n", OPT_DISABLERECURSION_STRING);
    printf_s("\n");
    printf_s(" Example: rffmpeg -path C:\\Users\\Music -fmt wav,aif -opts \"-c:a libmp3lame\" -ext mp3 --newfolder\n");
    printf_s("\n");
    printf_s(" (p.s.: you can run this as a console application by simply running it with no arguments)\n");
    printf_s("\n");
}

#endif