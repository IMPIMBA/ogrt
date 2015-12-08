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
#include "ogrt-log.h"
#include "ogrt-util.h"

/** vendor specific ELF note */
struct elf_note {
  int32_t name_size;
  int32_t desc_size;
  int32_t type;
  char    data[1];
} __attribute__((packed));
typedef struct elf_note elf_note;

struct ogrt_note {
  char name[8];
  u_char version[1];
  char uuid[37];
} __attribute__((packed));
typedef struct ogrt_note ogrt_note;

/* function prototypes */
int ogrt_read_signature(const char *note, char *ret_version, char **ret_signature);
int handle_program_header(struct dl_phdr_info *info, size_t size, void *data);
void *ogrt_get_loaded_so();

#endif
