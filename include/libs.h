#ifndef H_LIBS
#define H_LIBS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>
#include <time.h>

/* OS-dependent headers */
#ifdef _WIN32
    #include <windows.h>
    #include <fcntl.h>
    #include <io.h>
#endif

#endif // H_LIBS