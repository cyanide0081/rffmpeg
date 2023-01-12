#include "libs.h"

/* Pior programa que existe B) */

/* TODO: implement recursion, new folder option and delete option */
/* TODO: implement specific case where overwriting a file of same input fmt as output requires a temp file to be created and renamed afterwards */
/* TODO: fix opts parsing on console mode */
/* TODO: utf-8 characters break the parsing */

int start_main(char *args[], const bool opts[]);

int main(int argc, const char *argv[]) {
    char *args[MAX_ARGS] = { NULL };
    bool opts[MAX_OPTS] = { false };
    int exit_code;

    SetConsoleOutputCP(_codepage); // UTF-8 codepage

    /* Parse from arguments or start the hud mode */
    if (argc > 1) {
        parse_args(argc, argv, args);
        parse_opts(argc, argv, opts);

        printf("%s%s%s\n", CHARCOLOR_RED, full_title, COLOR_DEFAULT);
    } else {
        open_console(args, opts);
    }

    exit_code = start_main(args, opts); // Main loop

    for (int i = 0; args[i]; ++i)
        free(args[i]);

    putchar('\n');

    return exit_code;
}

int start_main(char *args[], const bool opts[]) {
    char in_path[PATH_MAX], out_path[PATH_MAX], parameters[BUFFER];
    char in_format[BUFFER], out_format[SHORTBUF];
    char current_path[PATH_MAX], next_path[PATH_MAX];

    /* Check for help option first */
    if (opts[OPT_HELP]) {
        display_help();
        return EXIT_SUCCESS;
    }

    if (handle_errors(args))
        exit(EXIT_FAILURE);

    /* Transfer args */
    strncpy(in_path,    args[ARG_PATH],   PATH_MAX);
    strncpy(parameters, args[ARG_PARAMS], BUFFER);
    strncpy(in_format,  args[ARG_FORMAT], BUFFER);
    strncpy(out_format, args[ARG_OUTPUT], SHORTBUF);

    /* Check if path was provided */
    if (strcmp(in_path, IDENTIFIER_NO_PATH) == 0)
        GetCurrentDirectoryA(PATH_MAX, in_path);

    /* Tokenize input formats */
    char in_formats[SHORTBUF][SHORTBUF];
    char *saveptr;
    const char *delim = ", ";
    int index = -1, fmts = 0;
    char *token = strtok_r(in_format, delim, &saveptr);

    for (int i = 0; token != NULL; ++i, ++fmts, token = strtok_r(NULL, delim, &saveptr))
        strncpy(in_formats[i], token, SHORTBUF);

    /* Append '.' to extensions */
    for (int i = 0; i < fmts; ++i)
        attach_dot(in_formats[i],  ".");

    attach_dot(out_format, ".");

    /* Temporary out_path hardcode */
    strncpy(out_path, in_path, PATH_MAX);

    DIR *dir = opendir(in_path);
    struct dirent *entry;

    putchar('\n');

    if (!dir) {
        fprintf(stderr, "%sERROR: %sCouldn't open \'%s\'\n", CHARCOLOR_RED, CHARCOLOR_WHITE, in_path);
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        const char *filename = entry->d_name;
        char filename_noext[FILENAME_MAX] = { '\0' };
        bool is_format = false;
        const char *opt_overwrite = "";

        /* Check for correct extension and skip ./.. */
        if (!strcmp(".", filename) || !strcmp("..", filename))
            continue;

        for (int i = 0; i < fmts; ++i) {
            if (strstr(filename, in_formats[i]) != NULL) {
                is_format = true;

                /* Trim extension from filename */
                strncpy_s(filename_noext, FILENAME_MAX, filename, strstr(filename, in_formats[i]) - filename);
                break;
            }
        }

        if (!is_format)
            continue;

        if (opts[OPT_OVERWRITE])
            opt_overwrite = "-y";
        else
            avoid_overwriting(filename_noext, out_format, in_path);

        char command[LONGBUF];

        snprintf(command, LONGBUF, "ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s%s\"", 
        opt_overwrite, in_path, filename, parameters, out_path, filename_noext, out_format);
        
        system(command);     
        putchar('\n');

        // /* Make new path for recursion */
        // strcpy(next_path, current_path);
        // strcat(next_path, "\\");
        // strcat(next_path, entry->d_name);

        // start_main();
    }
    closedir(dir);

    return EXIT_SUCCESS;
}