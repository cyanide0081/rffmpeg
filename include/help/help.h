#ifndef H_HELP_PAGE
#define H_HELP_PAGE

#include "../libs.h"

static void displayHelp(void) {
    wprintf_s(u" Use me to automate an ffmpeg conversion inside a directory and its subfolders\n");
    wprintf_s(u"\n");
    wprintf_s(u" Arguments:\n");
    wprintf_s(u"\t%-25lsthe directory containing your input files (defaults to current working path if left empty)\n", u"-path <path>");
    wprintf_s(u"\t%-25lsthe file format(s) you want to find and convert, separated by commas (e.g.: mp4,mov)\n", u"-in <format(s)>");
    wprintf_s(u"\t%-25lsthe ffmpeg conversion-related options (e.g.: -c:v libx264 -c:a aac -f matroska)\n", u"-opts <options>");
    wprintf_s(u"\t%-25lsthe extension of your output files (e.g.: mkv)\n", u"-out <extension>");
    wprintf_s(u"\n");
    wprintf_s(u" Additional flags:\n");
    wprintf_s(u"\t%-25lsstore converted files in a new folder - you can choose a folder name with '--newfolder=foldername'\n", OPT_MAKENEWFOLDER_STRING);
    wprintf_s(u"\t%-25lsauto-overwrite existing output files (default behaviour appends an index)\n", OPT_FORCEOVERWRITE_STRING);
    wprintf_s(u"\t%-25lsdelete original files after conversion (PERMANENT) \n", OPT_DELETEOLDFILES_STRING);
    wprintf_s(u"\t%-25lsdisable recursive subfolder search\n", OPT_DISABLERECURSION_STRING);
    wprintf_s(u"\n");
    wprintf_s(u" Example: rffmpeg -path C:\\Users\\Music -fmt wav,aif -opts \"-c:a libmp3lame\" -ext mp3 --newfolder\n");
    wprintf_s(u"\n");
    wprintf_s(u" (p.s.: you can run this as a console application by simply running it with no arguments)\n");
    wprintf_s(u"\n");
}

#endif // H_HELP_PAGE