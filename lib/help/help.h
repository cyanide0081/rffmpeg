#ifndef H_HELP_PAGE
#define H_HELP_PAGE

#include "../libs.h"

static void displayHelp(void) {
    printf(" Use me to automate an ffmpeg conversion inside a directory and its subfolders\n\n");

    printf(" Arguments:\n");
    printf("\t%-25sthe directory containing your input files (defaults to current working path if\
 left empty)\n", "-path <path(s)>");
    printf("\t%-25sthe file format(s) you want to find and convert, separated by commas\
 (e.g.: mp4,mov)\n", "-in <format(s)>");
    printf("\t%-25sthe ffmpeg conversion-related options (e.g.: -c:v libx264 -c:a aac -f\
 matroska)\n", "-opts <options>");
    printf("\t%-25sthe extension of your output files (e.g.: mkv)\n\n", "-out <extension>");

    printf(" Additional flags:\n");
    printf("\t%-25sstore converted files in a new subfolder created inside their respective\
 directories - you can choose a folder name with '--newfolder=foldername'\n",
     OPT_NEWFOLDER_STRING);
    printf("\t%-25sstore converted files in a separate directory - new or existing\
 (the syntax is the same as the newfolder option's)\n", OPT_NEWPATH_STRING);
    printf("\t%-25sauto-overwrite existing output files (default behaviour appends an index)\n",
     OPT_OVERWRITE_STRING);
    printf("\t%-25sdelete original files after conversion (PERMANENT) \n", OPT_CLEANUP_STRING);
    printf("\t%-25sdisable recursive subfolder search\n\n", OPT_NORECURSION_STRING);

    printf(" Example: rffmpeg -path C:\\Users\\Music -in wav,aif -opts \"-c:a libmp3lame\"\
 -out mp3 --newfolder\n\n");

    printf(" (p.s.: you can run this as a console application by simply running it with no\
 arguments)\n\n");
}

#endif // H_HELP_PAGE
