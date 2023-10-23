#ifndef H_HELP_PAGE
#define H_HELP_PAGE

#define HELP_PAGE                                                                      \
    " Use me to automate an ffmpeg conversion in one or more directories\n"            \
    " and organize its output automatically\n\n"                                       \
                                                                                       \
    " USAGE\n\n"                                                                       \
                                                                                       \
    " rffmpeg [PATH(S)] -i [IN-FMT(S)] -p [PARAMS] -o [OUT-FMT]\n\n"                   \
                                                                                       \
    " ARGUMENTS\n\n"                                                                   \
                                                                                       \
    " -i [FORMAT(S)]     the file format(s) you want to find and convert,\n"           \
    "                    separated by commas (e.g.: mp4,mov)\n"                        \
    " -p [PARAMS]        additional ffmpeg conversion options\n"                       \
    "                    (e.g.: \"-c:v libx264 -c:a aac -f matroska\"\n"               \
    " -o [FORMAT]        the extension of your output files (e.g.: mkv)\n\n"           \
                                                                                       \
    " NOTE: if you don't provide at least one input directory, it'll default\n"        \
    " to your current working directory\n\n"                                           \
                                                                                       \
    " ADDITIONAL FLAGS\n\n"                                                            \
                                                                                       \
    " -ow                silently overwrite conflicting output files\n"                \
    "                    (default behaviour appends a 3-digit index)\n"                \
    " -cl                delete original files after conversion (PERMANENT)\n"         \
    " -rn                disable recursive subfolder scanning\n"                       \
    " -subfolder         store output files in a relative subfolder\n"                 \
    "                    (you can give it a name with '-subfolder:[NAME]')\n"          \
    " -outpath:[PATH]    store output files in a separate directory\n"                 \
    "                    (doesn't preserve the directory structure)\n"                 \
    " -nt:[THREADS]      custom number of threads to be spawned when converting\n"     \
    "                    (must be between [1, (2 * system threads)] (inclusive))\n\n"  \
                                                                                       \
    " EXAMPLE\n\n"                                                                     \
                                                                                       \
    " rffmpeg ~/Music ~/Desktop -i wav,aif -p \"-b:a 320k\" -o mp3 -rn -subfolder\n\n" \
                                                                                       \
    " NOTE: you can run this as a console application if you give it no arguments)\n\n"\

#endif // H_HELP_PAGE
