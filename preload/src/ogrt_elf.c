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

#define OGRT_SECTION_NAME (".note.ogrt.info")

int walk(char *p) {
  int32_t name_size = *((int32_t *)p);
  int32_t desc_size = *((int32_t *)(p+4));
  int32_t type      = *((int32_t *)(p+8));
  char *name         = p+12;
  char *desc         = p+12+(name_size)+(4-(name_size%4));

  if(type == 0x4f475254) {
    printf("\nname=%p - size=%d, desc=%p - size=%d\n", (void *)name, name_size, (void *)desc, desc_size);
    printf("ogrt: %s\n", desc);
    for(int i=0; i < desc_size; i++){
      printf("%c", *(desc+i));
    }
  }

#if 0
  printf("%d: %s; hex: ", name_size, name);
  printf("\nname=%p - size=%d, desc=%p - size=%d\n", (void *)name, name_size, (void *)desc, desc_size);
  for(int i=0; i < desc_size; i++){
    printf("%x", *(desc+i));
  }
  printf("\n");
#endif
  return desc_size+name_size+12;
}

int
callback(struct dl_phdr_info *info, size_t size, void *data)
{
  int j;

  printf("name=%s (%d segments)\n", info->dlpi_name,info->dlpi_phnum);

  for (j = 0; j < info->dlpi_phnum; j++){
      printf("\t\t header %2d: address=%10p a=%10p s=%ld\n", j, (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr), (void *)info->dlpi_addr, info->dlpi_phdr[j].p_filesz);
      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_DYNAMIC) {
        printf("DYN");
      }
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE) {
        printf("NOTE");
        char *p = info->dlpi_addr + program_header->p_vaddr;
        if(p != NULL) {
          int i = 0;
          while(i < program_header->p_memsz) {
            i += walk(p+i);
          }
        }
      }
  }
  return 0;
}

int ogrt_get_loaded_so()
{
  dl_iterate_phdr(callback, NULL);
}

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
