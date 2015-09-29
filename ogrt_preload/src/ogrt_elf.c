#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ogrt.h"

int ogrt_read_info(const char *filename)
{
    int fd;
    Elf *e;
    char *name;
    Elf_Scn *scn;
    GElf_Shdr shdr;
    size_t shstrndx;
    size_t offset = 0;
    char buf[38];

    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EXIT_FAILURE, "ELF library initialization failed: %s", elf_errmsg(-1));

    if ((fd = open(filename, O_RDONLY, 0)) < 0)
        err(EXIT_FAILURE, "open \%s\" failed", filename);

    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EXIT_FAILURE, "elf_begin() failed: %s.", elf_errmsg(-1));

    if (elf_kind(e) != ELF_K_ELF)
        return 1;

    if (elf_getshdrstrndx(e, &shstrndx) != 0)
        errx(EXIT_FAILURE, "elf_getshdrstrndx() failed: %s.", elf_errmsg(-1));

    scn = NULL;
    while ((scn = elf_nextscn(e, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) != &shdr)
            errx(EXIT_FAILURE, "getshdr() failed: %s.", elf_errmsg(-1));

        if ((name = elf_strptr(e, shstrndx, shdr.sh_name)) == NULL)
            errx(EXIT_FAILURE, "elf_strptr() failed: %s.", elf_errmsg(-1));

        if(strcmp(name, OGRT_SECTION_NAME)== 0) {
          printf("OGRT section found! \n");
          offset = shdr.sh_offset;
        }
    }

    (void) elf_end(e);

    if(offset != 0) {
      lseek(fd, offset, SEEK_SET);
      int ret = read(fd, buf, 38);
      if(ret != 38) {
        err(EXIT_FAILURE, "read failed");
      }
      printf("section contains: %s \n", buf);
      (void) close(fd);
      return 0;
    } else {
      printf("OGRT section not found. \n");
      (void) close(fd);
      return 1;
    }
}
