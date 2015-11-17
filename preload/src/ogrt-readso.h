#ifndef OGRT_READSO_H_INCLUDED
#define OGRT_READSO_H_INCLUDED

/* definitions */
#define _GNU_SOURCE

#define OGRT_STAMP_SUPPORTED_VERSION (0x01)

/* includes */
#include <stdio.h>
#include <stdarg.h>
#include <link.h>
#include <dlfcn.h>
#include <gelf.h>
#include <uuid/uuid.h>
#include "ogrt-log.h"
#include "ogrt-fileutil.h"

/* function prototypes */
int read_note(const char *note, char *ret_version, uuid_t ret_uuid);
int handle_program_header(struct dl_phdr_info *info, size_t size, void *data);
void *ogrt_get_loaded_so();

#endif
