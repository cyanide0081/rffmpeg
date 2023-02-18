#ifndef H_LIBS
#define H_LIBS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

/* OS-dependent headers */
#ifdef _WIN32
    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/time.h>
#endif

#endif // H_LIBS