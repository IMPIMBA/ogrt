#ifndef OGRT_LOG_H_INCLUDED
#define OGRT_LOG_H_INCLUDED

/* definitions */
#define _GNU_SOURCE

/* includes */
#include <stdio.h>
#include <stdarg.h>
#include "ogrt-main.h"

#define OGRT_LOG_NOTHING  (0)
#define OGRT_LOG_FATAL    (1)
#define OGRT_LOG_ERR      (2)
#define OGRT_LOG_WARN     (3)
#define OGRT_LOG_INFO     (4)
#define OGRT_LOG_DBG      (5)

#define Log(level, ...) do {  \
                              if (level <= ogrt_log_level) { \
                                                                fprintf(ogrt_log_file,"%s: ", CMDLINE_PARSER_PACKAGE); \
                                                                fprintf(ogrt_log_file, __VA_ARGS__); \
                                                                fflush(ogrt_log_file); \
                                                            } \
                          } while (0)

extern FILE *ogrt_log_file;
extern int  ogrt_log_level;

#endif
