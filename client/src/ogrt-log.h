#ifndef OGRT_LOG_H_INCLUDED
#define OGRT_LOG_H_INCLUDED

/* definitions */
#define _GNU_SOURCE

/* includes */
#include <stdio.h>
#include <stdarg.h>
#include "ogrt-main.h"

/* function prototypes */
#ifdef OGRT_DEBUG
  void ogrt_log_debug(const char *msg, ...);
#else
  void ogrt_log_debug(__attribute__((unused)) const char *msg, ...);
#endif

#endif
