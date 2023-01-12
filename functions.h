#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include "libs.h"

void setup(void);
void prompt(const char *fmt);

size_t attach_dot(char *str, char *control);

int handle_errors(char **args);
void print_error(const char *msg);

char **parse_args(int count, const char *args[], char *dest[]);
bool *parse_opts(int count, const char *args[], bool dest[]);

int avoid_overwriting(char *pure_filename, char *out_format, char *path);

#endif