#ifndef H_LIBS
#define H_LIBS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
// #include <uchar.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/time.h>

/* OS-dependent headers */
#ifdef _WIN32

#include <windows.h>
#include <shellapi.h>
#include <fcntl.h>
#include <io.h>

#else

#include <unistd.h>
#include <sys/wait.h>

#endif

#endif // H_LIBS
