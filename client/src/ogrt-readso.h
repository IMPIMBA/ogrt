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

/* data structures */
struct elf_note {
  int32_t name_size;
  int32_t desc_size;
  int32_t type;
  uint8_t data[1];
} __attribute__((packed));
typedef struct elf_note elf_note;

struct ogrt_note {
  uint8_t name[8]; /* "OGRT" plus null terminator, plus padding to 4 byte boundary */
  uint8_t version;
  uint8_t uuid[37];
} __attribute__((packed));
typedef struct ogrt_note ogrt_note;

struct so_infos {
  int32_t size;
  int32_t index;
  OGRT__SharedObject shared_objects[1];
};
typedef struct so_infos so_infos;

/* function prototypes */
int ogrt_read_signature(const char *note, uint8_t *ret_version, char **ret_signature);
int handle_program_header(struct dl_phdr_info *info, size_t size, void *data);
so_infos *ogrt_get_loaded_so();

#endif
