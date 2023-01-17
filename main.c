#include "libs.h"

/* Pior programa que existe B) */

/* TODO: implement recursion, new folder option and delete option */
/* TODO: implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO: fix options parsing on console mode */
/* TODO: utf-8 characters break the parsing */

int runMainLoop(char *arguments[], const bool options[]);

int main(int argc, const char *argv[]) {
    char *arguments[MAX_ARGS] = { NULL };
    bool options[MAX_OPTS] = { false };
    int exitCode;

    SetConsoleOutputCP(_codepage); // UTF-8 codepage

    /* Parse from arguments or start the hud mode */
    if (argc > 1) {
        parseArguments(argc, argv, arguments);
        parseOptions(argc, argv, options);

        printf("%s%s%s\n", CHARCOLOR_RED, fullTitle, COLOR_DEFAULT);
    } else {
        runInConsoleMode(arguments, options);
    }

    exitCode = runMainLoop(arguments, options); // Main loop

    for (int i = 0; arguments[i]; ++i)
        free(arguments[i]);

    putchar('\n');

    return exitCode;
}

int runMainLoop(char *arguments[], const bool options[]) {
    char inputPath[PATH_MAX], outputPath[PATH_MAX], parameters[BUFFER];
    char inputFormat[BUFFER], outputFormat[SHORTBUF];
    char currentPath[PATH_MAX], nextPath[PATH_MAX];

    /* Check for help option first */
    if (options[OPT_HELP]) {
        displayHelp();
        return EXIT_SUCCESS;
    }

    if (handleErrors(arguments))
        exit(EXIT_FAILURE);

    /* Transfer arguments */
    strncpy(inputPath,    arguments[ARG_PATH],   PATH_MAX);
    strncpy(parameters, arguments[ARG_PARAMS], BUFFER);
    strncpy(inputFormat,  arguments[ARG_FORMAT], BUFFER);
    strncpy(outputFormat, arguments[ARG_OUTPUT], SHORTBUF);

    /* Check if path was provided */
    if (strcmp(inputPath, IDENTIFIER_NO_PATH) == 0)
        GetCurrentDirectoryA(PATH_MAX, inputPath);

    /* Tokenize input formats */
    char inputFormats[SHORTBUF][SHORTBUF];
    char *savePointer;
    const char *delimiter = ", ";
    int index = -1, numberOfFormats = 0;
    char *token = strtok_r(inputFormat, delimiter, &savePointer);

    for (int i = 0; token != NULL; ++i, ++numberOfFormats, token = strtok_r(NULL, delimiter, &savePointer))
        strncpy(inputFormats[i], token, SHORTBUF);

    /* Append '.' to extensions */
    for (int i = 0; i < numberOfFormats; ++i)
        appendDotToString(inputFormats[i],  ".");

    appendDotToString(outputFormat, ".");

    /* Temporary out_path hardcode */
    strncpy(outputPath, inputPath, PATH_MAX);

    DIR *directory = opendir(inputPath);
    struct dirent *entry;

    putchar('\n');

    if (!directory) {
        fprintf(stderr, "%sERROR: %sCouldn't open \'%s\'\n", CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath);
        return EXIT_FAILURE;
    }

    while ((entry = readdir(directory)) != NULL) {
        const char *filename = entry->d_name;
        char filenameWithoutExtension[FILENAME_MAX] = { '\0' };
        bool isOfCurrentFormat = false;
        const char *optionOverwrite = "";

        /* Check for correct extension and skip ./.. */
        if (!strcmp(".", filename) || !strcmp("..", filename))
            continue;

        for (int i = 0; i < numberOfFormats; ++i) {
            if (strstr(filename, inputFormats[i]) != NULL) {
                isOfCurrentFormat = true;

                /* Trim extension from filename */
                strncpy_s(filenameWithoutExtension, FILENAME_MAX, filename, strstr(filename, inputFormats[i]) - filename);
                break;
            }
        }

        if (!isOfCurrentFormat)
            continue;

        if (options[OPT_OVERWRITE])
            optionOverwrite = "-y";
        else
            preventFilenameOverwrites(filenameWithoutExtension, outputFormat, inputPath);

        char command[LONGBUF];

        snprintf(command, LONGBUF, "ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s%s\"", 
        optionOverwrite, inputPath, filename, parameters, outputPath, filenameWithoutExtension, outputFormat);
        
        system(command);     
        putchar('\n');

        // /* Make new path for recursion */
        // strcpy(next_path, current_path);
        // strcat(next_path, "\\");
        // strcat(next_path, entry->d_name);

        // start_main();
    }
    closedir(directory);

    return EXIT_SUCCESS;
}