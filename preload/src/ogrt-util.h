#ifndef OGRT_FILEUTIL_H_INCLUDED
#define OGRT_FILEUTIL_H_INCLUDED

/* definitions */
#define _GNU_SOURCE

/* includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <stdbool.h>

/* function prototypes */
bool ogrt_env_enabled(char *env_var);
char *ogrt_normalize_path(const char *path);
char *ogrt_get_binpath(const pid_t pid);

#endif
