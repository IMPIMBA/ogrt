#include "ogrt-main.h"
#include <gelf.h>
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ogrt-readso.h"

int ogrt_read_info(const char *filename)
{
    if (elf_version(EV_CURRENT) == EV_NONE) {
        errx(EXIT_FAILURE, "ELF library initialization failed: %s", elf_errmsg(-1));
    }

    int fd;
    if ((fd = open(filename, O_RDONLY, 0)) < 0) {
        err(EXIT_FAILURE, "open \%s\" failed", filename);
    }

    Elf *elf;
    if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
        errx(EXIT_FAILURE, "elf_begin() failed: %s.", elf_errmsg(-1));
    }

    if (elf_kind(elf) != ELF_K_ELF) {
        return 1;
    }

    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        errx(EXIT_FAILURE, "elf_getshdrstrndx() failed: %s.", elf_errmsg(-1));
    }

    int notes_found = 0;
    Elf_Scn *scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr)
            errx(EXIT_FAILURE, "getshdr() failed: %s.", elf_errmsg(-1));

        char *name;
        if ((name = elf_strptr(elf, shstrndx, shdr.sh_name)) == NULL)
            errx(EXIT_FAILURE, "elf_strptr() failed: %s.", elf_errmsg(-1));

        if(shdr.sh_type == SHT_NOTE) {
          Elf_Data *section_data = NULL;
          section_data = elf_getdata(scn, section_data);
          if(section_data == NULL) {
            errx(EXIT_FAILURE, "elf_getdata() failed: %s.", elf_errmsg(-1));
          }

          elf_note *note = (elf_note *)(section_data->d_buf);
          if(note->type == OGRT_ELF_NOTE_TYPE) {
            notes_found++;
            ogrt_note *og_note = (ogrt_note *)note->data;
            printf("OGRT signature in section %s\n", name);
            printf("allocatable:\t%s\n", shdr.sh_flags & SHF_ALLOC ? "yes" : "no");
            printf("version:\t%d\n", og_note->version);
            printf("name:\t\t%s\n", og_note->name);
            printf("uuid:\t\t%s\n", og_note->uuid);
          }
        }
    }

    (void) elf_end(elf);
    return notes_found;
}

/**
 * Function used when executing the shared library as executable.
 * Display various information on how the program was compiled.
 */
int main(int argc, char *argv[]) {
  struct gengetopt_args_info ai;
  if (cmdline_parser(argc, argv, &ai) != 0) {
      exit(1);
  }

  if(argc == 1) {
    cmdline_parser_print_help();
  }

  if(ai.generate_signature_given) {
    char hostname[HOST_NAME_MAX+1];
    if(gethostname(hostname, sizeof(hostname)) != 0) {
      fprintf(stderr, "failed to get hostname\n");
      return 1;
    }
    //printf("Host name: %s\n", hostname);

    //struct passwd *pwd_entry = getpwuid(getuid());
    //printf("User name: %s\n", pwd_entry->pw_name);

    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse_lower(uuid, template_signature+0x55);
    //fprintf(stderr, "UUID: %s\n", template_signature+0x55);

    for(unsigned int i=0; i < template_signature_len; i++) {
      printf("%c", template_signature[i]);
    }
  } else if(ai.show_signature_given) {
    return ogrt_read_info(ai.show_signature_arg);
  }


}
