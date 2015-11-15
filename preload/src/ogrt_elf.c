#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <link.h>
#include <errno.h>
#include "ogrt.h"

extern char *program_invocation_name;

typedef struct so_info {
  char *path;
  void *stamp;
} so_info;

/**
 * Read a "vendor specific ELF note".
 * Only documentation I could find: http://www.netbsd.org/docs/kernel/elf-notes.html
 * Takes a pointer to the beginning of the note and returns the total size of the note.
 */
int read_note(const char *p) {
  int32_t name_size = *((int32_t *)p);
  int32_t desc_size = *((int32_t *)(p+4));
  int32_t type      = *((int32_t *)(p+8));
  char *name         = (char *)p+12;
  char *desc         = (char *)p+12+(name_size)+(4-(name_size%4));

  if(type == OGRT_ELF_NOTE_TYPE) {
    fprintf(stderr, "OGRT: found stamp with content: %s\n", desc);
  }

  return desc_size+name_size+12;
}

int handle_program_header(struct dl_phdr_info *info, size_t size, void *data)
{
  if(strlen(info->dlpi_name) > 0) {
    fprintf(stderr, "OGRT: \t\t %s\n", info->dlpi_name);
  }
#ifdef OGRT_DEBUG
  printf("name=%s (%d segments)\n", info->dlpi_name,info->dlpi_phnum);
#endif
  for (int j = 0; j < info->dlpi_phnum; j++){
#ifdef OGRT_DEBUG
      printf("\t\t header %2d: address=%10p a=%10p s=%ld\n", j, (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr), (void *)info->dlpi_addr, info->dlpi_phdr[j].p_filesz);
#endif
      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE) {
#ifdef OGRT_DEBUG
        printf("NOTE");
#endif
        char *notes = (char *)(info->dlpi_addr + program_header->p_vaddr);
        if(notes != NULL) {
          u_int offset = 0;
          while(offset < program_header->p_memsz) {
            offset += read_note(notes + offset);
          }
        }
      }
  }
  return 0;
}

void ogrt_get_loaded_so()
{
  fprintf(stderr, "OGRT: Displaying loaded libraries for pid %d (%s):\n", getpid(), program_invocation_name);
  dl_iterate_phdr(handle_program_header, NULL);
}

#if 0
int ogrt_read_info(const char *filename)
{
    int fd;
    Elf *elf = NULL;
    size_t section_header_stringtable_size;

    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization failed: %s", elf_errmsg(-1));

    if ((fd = open(filename, O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open \%s\" failed", filename);

    if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "elf_begin() failed: %s.", elf_errmsg(-1));

    if (elf_kind(elf) != ELF_K_ELF)
        return 1;

    if (elf_getshdrstrndx(elf, &section_header_stringtable_size) != 0)
        errx(EXIT_FAILURE, "elf_getshdrstrndx() failed: %s.", elf_errmsg(-1));

    char *name = NULL;
    Elf_Scn *section = NULL;
    GElf_Shdr section_header;

    while ((section = elf_nextscn(elf, section)) != NULL) {
        if (gelf_getshdr(section, &section_header) != &section_header)
            errx(EXIT_FAILURE, "getshdr() failed: %s.", elf_errmsg(-1));

        if ((name = elf_strptr(elf, section_header_stringtable_size, section_header.sh_name)) == NULL)
            errx(EXIT_FAILURE, "elf_strptr() failed: %s.", elf_errmsg(-1));

        if(strcmp(name, OGRT_SECTION_NAME) == 0) {
          printf("OGRT section found! \n");
          Elf_Data *data = NULL;
          while ((data = elf_getdata(section, data)) != NULL) {
            printf("ogrt: %s\n", (char *)data->d_buf);
          }

        }
        if(section_header.sh_type == SHT_DYNAMIC) {
          printf("Dynamic section found. \n");
          Elf_Data *data = NULL;
          GElf_Dyn dyn;
          while ((data = elf_getdata(section, data)) != NULL) {
            for(int i_dyn = 0; gelf_getdyn(data, i_dyn, &dyn) != NULL && dyn.d_tag != DT_NULL; i_dyn++) {
              if(dyn.d_tag == DT_NEEDED) {
                printf("requires library: %s\n", elf_strptr(elf, section_header.sh_link, dyn.d_un.d_ptr));
              }
            }
          }
        }
    }

    (void) elf_end(elf);
    return 0;
}
#endif
