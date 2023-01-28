#include "functions.h"

void printError(const char *msg) {
    fprintf(stderr, u8"%sERROR: %s%s%s\n\n", CHARCOLOR_RED, CHARCOLOR_WHITE, msg, COLOR_DEFAULT);
}

void parseArguments(const int count, const char *rawArguments[], char *parsedArguments[], bool parsedOptions[], bool parseArguments, bool parseOptions) {
    if (parseArguments) {
        /* fmt: -i <path> -f <container> -p <params> -o <container> */
        for (size_t i = 1; i < count; ++i) {
            if (strcmp(rawArguments[i], u8"-path") == 0) {
                strncpy_s(parsedArguments[ARG_INPATH], PATHBUF - 1, rawArguments[++i], PATHBUF);
            } else if (strcmp(rawArguments[i], u8"-fmt") == 0) {
                strncpy_s(parsedArguments[ARG_INFORMAT], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (strcmp(rawArguments[i], u8"-opts") == 0) {
                strncpy_s(parsedArguments[ARG_INPARAMETERS], BUFFER - 1, rawArguments[++i], BUFFER);
            } else if (strcmp(rawArguments[i], u8"-ext") == 0) {
                strncpy_s(parsedArguments[ARG_OUTFORMAT], SHORTBUF - 1, rawArguments[++i], SHORTBUF);
            }
        }
    }

    if (parseOptions) {
        /* fmt: --help, --newfolder=foldername, --delete, --norecursion, --overwrite, */
        for (size_t i = 0; i < count; ++i) {
            if (strcmp(rawArguments[i], OPT_DISPLAYHELP_STRING) == 0) {
                parsedOptions[OPT_DISPLAYHELP] = true;
            } else if (strstr(rawArguments[i], OPT_MAKENEWFOLDER_STRING)) {
                parsedOptions[OPT_MAKENEWFOLDER] = true;

                char *argumentBuffer = strdup(rawArguments[i]); // duplicate argument string for analysis

                char *parserState;
                char *delimiter = u8"=";
                char *token = strtok_s(argumentBuffer, delimiter, &parserState);

                /* If there's an '=' sign, pass the string after it to the foldername argument */
                if ((token = strtok_s(NULL, delimiter, &parserState)) != NULL) {
                    parsedOptions[OPT_CUSTOMFOLDERNAME] = true;
                    
                    strcpy_s(parsedArguments[ARG_NEWFOLDERNAME], PATHBUF, token);
                }

                free(argumentBuffer);
            } else if (strcmp(rawArguments[i], OPT_DELETEOLDFILES_STRING) == 0) {
                parsedOptions[OPT_DELETEOLDFILES] = true;
            } else if (strcmp(rawArguments[i], OPT_DISABLERECURSION_STRING) == 0) {
                parsedOptions[OPT_DISABLERECURSION] = true;
            } else if (strcmp(rawArguments[i], OPT_FORCEOVERWRITE_STRING) == 0) {
                parsedOptions[OPT_FORCEOVERWRITE] = true;
            }
        }
    }
}

errno_t clearConsoleWindow(void) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD consoleMode = 0;

    if (!GetConsoleMode(handleToStdOut, &consoleMode)) {
        return GetLastError();
    }

    const DWORD originalConsoleMode = consoleMode;

    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(handleToStdOut, consoleMode)) {
        return GetLastError();
    }

    DWORD writtenCharacters = 0;
    PCSTR sequence = u8"\x1b]10d\x1b]1G";

    if (!WriteConsoleA(handleToStdOut, sequence, (DWORD)strlen(sequence), &writtenCharacters, NULL)) {
        SetConsoleMode(handleToStdOut, originalConsoleMode);
        return GetLastError();
    }

    SetConsoleMode(handleToStdOut, originalConsoleMode);

    return EXIT_SUCCESS;
}

errno_t resetConsoleMode(DWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!SetConsoleMode(handleToStdOut, originalConsoleMode)) {
        return GetLastError();
    }

    return EXIT_SUCCESS;
}

errno_t enableVirtualTerminalProcessing(PDWORD originalConsoleMode) {
    HANDLE handleToStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD consoleMode = 0;

    if (!GetConsoleMode(handleToStdOut, &consoleMode)) {
        return GetLastError();
    }

    *originalConsoleMode = consoleMode;

    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(handleToStdOut, consoleMode)) {
        return GetLastError();
    }

    return EXIT_SUCCESS;
}

int preventFilenameOverwrites(char *pureFilename, const char *outputFormat, const char *path) {
    char fileMask[PATHBUF];
    char fileNameNew[PATHBUF];

    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fileData;

    sprintf_s(fileMask, PATHBUF, u8"%s\\%s.%s", path, pureFilename, outputFormat);

    if ((fileHandle = FindFirstFileA(fileMask, &fileData)) != INVALID_HANDLE_VALUE) {
        size_t index = 0;

        do {
            sprintf_s(fileMask, BUFFER, u8"%s\\%s-%03d.%s", path, pureFilename, ++index, outputFormat);
        }   while ((fileHandle = FindFirstFileA(fileMask, &fileData)) != INVALID_HANDLE_VALUE);

        sprintf_s(fileNameNew, PATHBUF, u8"%s-%03d", pureFilename, index);
        strcpy_s(pureFilename, PATHBUF, fileNameNew);
    }

    return EXIT_SUCCESS;
}

errorCode_t handleErrors(char *arguments[]) {
    if (*arguments[ARG_INPATH] == 0) {       
        GetCurrentDirectoryA(PATHBUF, arguments[ARG_INPATH]);
    }

    if (*arguments[ARG_INFORMAT] == 0) {
        printError(u8"no input format (null)");
        return ERROR_NO_INPUT_FORMAT;
    }

    if (*arguments[ARG_OUTFORMAT] == 0) {
        printError(u8"no output format (null)");
        return ERROR_NO_OUTPUT_FORMAT;
    }

    return ERROR_NONE;
}

errorCode_t searchDirectory(const char *directory, char *arguments[], const bool *options, processInfo_t *runtimeData) {
    const char *inputPath         = directory == NULL ? arguments[ARG_INPATH] : directory;
    const char *inputFormatString = arguments[ARG_INFORMAT];
    const char *parameters        = arguments[ARG_INPARAMETERS];
    const char *outputFormat      = arguments[ARG_OUTFORMAT];
    const char *newFolderName     = arguments[ARG_NEWFOLDERNAME];

    size_t inputFormatIndex = 0;
    static size_t numberOfInputFormats = 0;
    static char inputFormats[SHORTBUF][SHORTBUF] = { IDENTIFIER_NO_FORMAT };

    /* Save cpu resources by only tokenizing formats once */
    if (strcmp(inputFormats[0], IDENTIFIER_NO_FORMAT) == 0 && numberOfInputFormats == 0) {
        char inputFormatStringBuffer[BUFFER];

        strcpy_s(inputFormatStringBuffer, BUFFER - 1, inputFormatString);

        char *parserState;
        char *token = strtok_s(inputFormatStringBuffer, u8", ", &parserState);

        while (token != NULL) {
            sprintf_s(inputFormats[numberOfInputFormats++], SHORTBUF - 1, u8".%s", token);
            token = strtok_s(NULL, u8", ", &parserState);
        }
    }
    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAA fileData;
    char pathMask[BUFFER];
    wchar_t pathMaskWide[BUFFER];

    sprintf_s(pathMask, BUFFER - 1, u8"%s\\*", inputPath);
    swprintf_s(pathMaskWide, BUFFER - 1, u"%ls\\*", inputPath);

    if ((fileHandle = FindFirstFileA(pathMask, &fileData)) == INVALID_HANDLE_VALUE) {
        fprintf_s(stderr, u8"%sERROR: %sCouldn't open \'%s\' (code: %s%lu%s)\n\n",
            CHARCOLOR_RED, CHARCOLOR_WHITE, inputPath, CHARCOLOR_RED, GetLastError(), CHARCOLOR_WHITE);

        return ERROR_FAILED_TO_OPEN_DIRECTORY;
    }

    do {
        /* Skip . and .. */
        if (strcmp(fileData.cFileName, u8".") == 0 || strcmp(fileData.cFileName, u8"..") == 0)
            continue;

        /* Perform recursive search (or not) */
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && options[OPT_DISABLERECURSION] == false) {
            char newPathMask[PATHBUF];

            sprintf_s(newPathMask, PATHBUF - 1, u8"%s\\%s", inputPath, fileData.cFileName);

            searchDirectory(newPathMask, arguments, options, runtimeData);

            continue;
        }

        bool isOfFormat = false;

        for (size_t i = 0; i < numberOfInputFormats; ++i) {
            if (strstr(fileData.cFileName, inputFormats[i]) != NULL) {
                isOfFormat = true;
                inputFormatIndex = i;
                break;
            }
        }

        if (!isOfFormat)
            continue;

        char *fileName = fileData.cFileName;
        char fileNameNoExtension[PATHBUF];
        char outputPath[PATHBUF];

        const char *overwriteOption = options[OPT_FORCEOVERWRITE] == true ? u8"-y" : u8"";

        strcpy_s(fileNameNoExtension, PATHBUF - 1, fileName);
        *(strstr(fileNameNoExtension, inputFormats[inputFormatIndex])) = L'\0'; 

        /* Make-a-subfolder-or-not part */
        if (options[OPT_MAKENEWFOLDER] == true) {
            char subFolderDirectory[PATHBUF];

            const char *folderName = options[OPT_CUSTOMFOLDERNAME] == true ? arguments[ARG_NEWFOLDERNAME] : outputFormat;

            sprintf_s(subFolderDirectory, PATHBUF, u8"%s\\%s", inputPath, folderName);
            CreateDirectoryA(subFolderDirectory, NULL);
            strcpy_s(outputPath, PATHBUF - 1, subFolderDirectory);
        } else {
            strcpy_s(outputPath, PATHBUF - 1, inputPath);
        }

        if (options[OPT_FORCEOVERWRITE] == false)
            preventFilenameOverwrites(fileNameNoExtension, outputFormat, outputPath);

        char ffmpegParameters[LONGBUF];

        sprintf_s(ffmpegParameters, LONGBUF, u8"ffmpeg -hide_banner %s -i \"%s\\%s\" %s \"%s\\%s.%s\"", 
            overwriteOption, inputPath, fileName, parameters, outputPath, fileNameNoExtension, outputFormat);
        
        /* Setup process info structures */
        STARTUPINFOA ffmpegStartupInformation = { sizeof(ffmpegStartupInformation) };
        PROCESS_INFORMATION ffmpegProcessInformation;

        /* Call ffmpeg and wait for it to finish */
        if (CreateProcessA(NULL, ffmpegParameters, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &ffmpegStartupInformation, &ffmpegProcessInformation)) {
            WaitForSingleObject(ffmpegProcessInformation.hProcess, INFINITE);

            CloseHandle(ffmpegProcessInformation.hProcess);
            CloseHandle(ffmpegProcessInformation.hThread);

            ++(runtimeData->convertedFiles);

            printf_s(u8"\n");
        }
        
        /* Keep or delete original files */
        if (options[OPT_DELETEOLDFILES] == true) {
            char inputFilePath[PATHBUF];

            sprintf_s(inputFilePath, PATHBUF, u8"%s\\%s", inputPath, fileName);

            if (DeleteFileA(inputFilePath)) {
                ++(runtimeData->deletedFiles);
            }
        }
    } while (FindNextFileA(fileHandle, &fileData));

    FindClose(fileHandle);

    return EXIT_SUCCESS;
}

void displayEndDialog(processInfo_t *processInformation) {
    if (processInformation->convertedFiles == 0) {
        printError(u8"No input files were found\n");
        printf_s(u8"\n");
    } else {
        printf_s(u8" %sDONE!%s\n", CHARCOLOR_RED, COLOR_DEFAULT);
        printf_s(u8"\n");
        printf_s(u8" %sConverted file(s): %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->convertedFiles, COLOR_DEFAULT);
        printf_s(u8" %sDeleted file(s): %s%llu%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->deletedFiles, COLOR_DEFAULT);
        printf_s(u8" %sExecution time: %s%.2lfs%s\n", CHARCOLOR_WHITE, CHARCOLOR_RED, processInformation->executionTime, COLOR_DEFAULT);
        printf_s(u8"\n");
    }
 }