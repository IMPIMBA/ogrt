#include "ogrt-readso.h"

/**
 * Read a "vendor specific ELF note".
 * Only documentation I could find: http://www.netbsd.org/docs/kernel/elf-notes.html
 * The signature is in the format:
 *                4 bytes
 * +-----------------------------------+
 * |             name_size             |
 * +-----------------------------------+
 * |             desc_size             |
 * +-----------------------------------+
 * |               type                |
 * +-----------------------------------+
 * |               name                |
 * +-----------------------------------+
 * | ver |            uuid             |
 * +-----------------------------------+
 *
 * All values are padded to 4 byte boundaries.
 * OGRT version is 1 byte, uuid is a null terminated string.
 */
int read_signature(const void *note, u_char *ret_version, char **ret_signature) {
  const elf_note *elf_note = note;

  if(elf_note->type == OGRT_ELF_NOTE_TYPE) {
    const ogrt_note *ogrt_note = (const struct ogrt_note *)&(elf_note->data);
    Log(OGRT_LOG_DBG, "\tfound ogrt note with size %d!\n", elf_note->desc_size);
    Log(OGRT_LOG_DBG, "[D] -> name %s (%10p)!\n", ogrt_note->name, ogrt_note->name);
    Log(OGRT_LOG_DBG, "[D] -> version %u (%10p)!\n", (u_int)(ogrt_note->version[0]), ogrt_note->version);
    Log(OGRT_LOG_DBG, "[D] -> signature %s (%10p)!", ogrt_note->uuid, ogrt_note->uuid);
    if(ogrt_note->version[0] == OGRT_STAMP_SUPPORTED_VERSION) {
      *ret_version = ogrt_note->version[0];
      *ret_signature = (char *)ogrt_note->uuid;
    }
  }

  return elf_note->desc_size + elf_note->name_size + 12;
}

/**
 * Process an ELF program header section (located in memory).
 */
int handle_program_header(struct dl_phdr_info *info, __attribute__((unused))size_t size, void *data)
{
  /** if there is a name, normalize it's path **/
  char *so_name = NULL;
  if(strlen(info->dlpi_name) > 0) {
    so_name = ogrt_normalize_path(info->dlpi_name);
    Log(OGRT_LOG_DBG, "[D] \t\t %s\n", so_name);
  } else {
    so_name = strdup(info->dlpi_name);
  }

  Log(OGRT_LOG_DBG, "[D] name=%s (%d segments)\n", info->dlpi_name, info->dlpi_phnum);

  so_infos *so_infos = data;

  OGRT__SharedObject *shared_object = &(so_infos->shared_objects[so_infos->index]);
  ogrt__shared_object__init(shared_object);
  shared_object->path = so_name;

  Log(OGRT_LOG_DBG, "[D] so_info: size %d, index %d\n", so_infos->size, so_infos->index);
  Log(OGRT_LOG_DBG, "[D] so_info: size %10p, index %10p\n", so_infos->size, so_infos->index);

  /** check all sections */
  for (int j = 0; j < info->dlpi_phnum; j++){
      Log(OGRT_LOG_DBG, "[D]\t\theader %2d: address=%10p phys=%10p size=%ld", j, (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr), (void *)info->dlpi_addr, info->dlpi_phdr[j].p_filesz);

      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE) {
        Log(OGRT_LOG_DBG, "\b\b\b\b\b\b      [NOTE]");

        char *notes = (char *)(info->dlpi_addr + program_header->p_vaddr);
        if(notes != NULL) {
          u_int offset = 0;
          u_char version = 0;
          char *signature = NULL;
          while(offset < program_header->p_memsz) {
            offset += read_signature(notes + offset, &version, &signature);
            shared_object->signature = signature;
          }
        }
      }

      Log(OGRT_LOG_DBG, "\b\b\b\b\b\b      \n");
  }
  so_infos->index += 1;
  return 0;
}

int count_program_header(__attribute__((unused)) struct dl_phdr_info *info, __attribute__((unused)) size_t size, void *data) {
  uint32_t *count = data;
  (*count)++;
  Log(OGRT_LOG_DBG, "[D] so_count: %u\n", *count);
  return 0;
}


so_infos *ogrt_get_loaded_so()
{
  Log(OGRT_LOG_DBG, "[D] Displaying loaded libraries for pid %d (%s):\n", getpid(), ogrt_get_binpath(getpid()));

  uint32_t so_count = 0;
  dl_iterate_phdr(count_program_header, (void *)&so_count);
  Log(OGRT_LOG_DBG, "[D] Total so_count: %u\n", so_count);

  Log(OGRT_LOG_DBG, "[D] sizeof(OGRT__SharedObject)=%d\n", sizeof(OGRT__SharedObject));
  so_infos *infos = malloc(sizeof(OGRT__SharedObject) * so_count + sizeof(int32_t) * 2);
  infos->size = so_count;
  infos->index = 0;
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
