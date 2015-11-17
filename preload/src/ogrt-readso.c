#include "ogrt-readso.h"

typedef struct so_info {
  char *path;
  uuid_t signature;
} so_info;

/**
 * Read a "vendor specific ELF note".
 * Only documentation I could find: http://www.netbsd.org/docs/kernel/elf-notes.html
 */
int read_note(const char *note, char *ret_version, uuid_t ret_uuid) {
  int32_t name_size = *((int32_t *)note);
  int32_t desc_size = *((int32_t *)(note+4));
  int32_t type      = *((int32_t *)(note+8));
  char *name         = (char *)note+12;
  char *version      = (char *)note+12+(name_size)+(4-(name_size%4));
  char *uuid_str         = version+1;

  if(type == OGRT_ELF_NOTE_TYPE && *version == OGRT_STAMP_SUPPORTED_VERSION) {
    ogrt_log_debug("\n[D] found signature %s!", uuid_str);
    *ret_version = *version;
    uuid_parse(uuid_str, ret_uuid);
  }

  return desc_size+name_size+12;
}

int handle_program_header(struct dl_phdr_info *info, size_t size, void *data)
{
  char *so_name = NULL;
  if(strlen(info->dlpi_name) > 0) {
    so_name = ogrt_normalize_path(info->dlpi_name);
    fprintf(stderr, "OGRT: \t\t %s\n", so_name);
  } else {
    so_name = strdup(info->dlpi_name);
  }

  ogrt_log_debug("[D] name=%s (%d segments)\n", info->dlpi_name, info->dlpi_phnum);

  int32_t *so_info_size = ((int32_t *)data);
  int32_t *so_info_index = ((int32_t *)data) + 1;
  so_info *so_infos = (so_info *)(so_info_index + 1);

  ogrt_log_debug("[D] so_info: size %d, index %d\n", *so_info_size, *so_info_index);
  (*so_info_index)++;
  for (int j = 0; j < info->dlpi_phnum; j++){
      ogrt_log_debug("[D]\t\theader %2d: address=%10p phys=%10p size=%ld", j, (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr), (void *)info->dlpi_addr, info->dlpi_phdr[j].p_filesz);

      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE) {
        ogrt_log_debug("\t[NOTE]");

        char *notes = (char *)(info->dlpi_addr + program_header->p_vaddr);
        if(notes != NULL) {
          u_int offset = 0;
          while(offset < program_header->p_memsz) {
            uuid_t uuid;
            uuid_clear(uuid);
            char version = 0;
            offset += read_note(notes + offset, &version, uuid);
            uuid_copy(so_infos[*so_info_index].signature, uuid);
            so_infos[*so_info_index].path = so_name;
          }
        }
      }

      ogrt_log_debug("\n");
  }
  return 0;
}

int count_program_header(struct dl_phdr_info *info, __attribute__((unused)) size_t size, void *data) {
  uint32_t *count = data;
  (*count)++;
  ogrt_log_debug("[D] so_count: %u\n", *count);
  return 0;
}

uuid_t ogrt_get_process_signature() {
  
}

void *ogrt_get_loaded_so()
{
  fprintf(stderr, "OGRT: Displaying loaded libraries for pid %d (%s):\n", getpid(), ogrt_get_binpath(getpid()));

  uint32_t so_count = 0;
  dl_iterate_phdr(count_program_header, (void *)&so_count);
  ogrt_log_debug("[D] Total so_count: %u\n", so_count);

  void *infos = malloc(sizeof(so_info) * so_count + sizeof(int32_t) * 2);
  int32_t *so_info_size = ((int32_t *)infos);
  int32_t *so_info_index = ((int32_t *)infos) + 1;
  *so_info_size = so_count;
  *so_info_index = 0;
  dl_iterate_phdr(handle_program_header, infos);

  return infos;
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
