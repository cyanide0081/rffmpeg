#ifndef HEADER_HELP
#define HEADER_HELP

#include "libs.h"

static void display_help(void) {
    printf("%s\n", full_title);
    printf(" Use me to automate an ffmpeg conversion throughout a whole directory\n");
    printf("\n");
    printf(" Arguments:\n");
    printf("   -i <path>          the path you want to search through\n");
    printf("   -f <format(s)>     the format(s) you want to find and convert (e.g.: mp4,mov)\n");
    printf("   -p <parameters>    the ffmpeg conversion options\n");
    printf("   -o <output>        the output container (e.g.: mkv)\n");
    printf("\n");
    printf(" Optional modes:\n");
    printf("   /n                 store the converted files in a new folder     (not implemented)\n");
    printf("   /d                 delete the original files after conversion    (not implemented)\n");
    printf("   /r                 search through subfolders                     (not implemented)\n");
    printf("   /y                 auto-overwrite clashing files (default behaviour appends an index)\n");
    printf("\n");
    printf(" Example usage: rffmpeg -i C:\\Users\\Music -f wav,aif -p \"-c:a libmp3lame\" -o mp3\n");
}

#endif