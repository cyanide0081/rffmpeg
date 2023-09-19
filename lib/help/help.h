#ifndef H_HELP_PAGE
#define H_HELP_PAGE

#include "../libs.h"

static void displayHelp(void) {
    printf(" Use me to automate an ffmpeg conversion in one or more directories\n");
    printf(" and organize its output automatically\n\n");

    printf(" ARGUMENTS:\n\n");


    printf("\t%-20sthe file format(s) you want to find and convert,\n", "-i [FORMATS]");
    printf("\t%-20sseparated by commas (e.g.: mp4,mov)\n", "");
    printf("\t%-20sthe ffmpeg conversion-related options\n", "-p [PARAMS]");
    printf("\t%-20s(e.g.: \"-c:v libx264 -c:a aac -f matroska\")\n", "");
    printf("\t%-20sthe extension of your output files (e.g.: mkv)\n\n", "-o [FORMAT]");

    printf(" NOTE: if you don't provide at least one input directory, it'll default to your\n current working directory\n\n");

    printf(" ADDITIONAL FLAGS:\n\n");

    printf("\t%-20sauto-overwrite existing output files\n", "-ow");
    printf("\t%-20s(default behaviour appends an index)\n", "");
    printf("\t%-20sdelete original files after conversion (PERMANENT) \n", "-cl");
    printf("\t%-20sdisable recursive subfolder scanning\n", "-rn");
    printf("\t%-20sstore converted files in a new subfolder\n", "-subfolder");
    printf("\t%-20screated inside their respective directories\n", "");
    printf("\t%-20syou can choose a name with '-subfolder=[NAME]')\n", "");
    printf("\t%-20sstore converted files in a separate directory\n\n", "-newpath=[PATH]");

    printf(" Example:\n\trffmpeg C:\\Users\\Music -i wav,aif -p \"-c:a libmp3lame -b:a 320k\" -o mp3\n -subfolder\n\n");

    printf(" (p.s.: you can run this as a console application with no arguments)\n\n");
}

#endif // H_HELP_PAGE
