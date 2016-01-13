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
int read_signature(const void *note, uint8_t *ret_version, char **ret_signature) {
  const elf_note *elf_note = note;

  if(elf_note->type == OGRT_ELF_NOTE_TYPE) {
    const ogrt_note *ogrt_note = (const struct ogrt_note *)&(elf_note->data);
    Log(OGRT_LOG_DBG, "\tfound ogrt note with size %d!\n", elf_note->desc_size);
    Log(OGRT_LOG_DBG, "[D] -> name %s (%10p)!\n", ogrt_note->name, ogrt_note->name);
    Log(OGRT_LOG_DBG, "[D] -> version %u (%10p)!\n", ogrt_note->version, &ogrt_note->version);
    Log(OGRT_LOG_DBG, "[D] -> signature %s (%10p)!", ogrt_note->uuid, ogrt_note->uuid);
    if(ogrt_note->version == OGRT_STAMP_SUPPORTED_VERSION) {
      *ret_version = ogrt_note->version;
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
  Log(OGRT_LOG_DBG, "[D] so_info: size %10p, index %10p\n", &(so_infos->size), &(so_infos->index));

  /** check all sections */
  for (int j = 0; j < info->dlpi_phnum; j++){
      Log(OGRT_LOG_DBG, "[D]\t\theader %2d: address=%10p phys=%10p size=%ld", j, (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr), (void *)info->dlpi_addr, info->dlpi_phdr[j].p_filesz);

      GElf_Phdr *program_header= (GElf_Phdr *)&(info->dlpi_phdr[j]);
      if(program_header->p_type != PT_NULL && program_header->p_type == PT_NOTE) {
        Log(OGRT_LOG_DBG, "\b\b\b\b\b\b      [NOTE]");

        uint8_t *notes = (uint8_t *)(info->dlpi_addr + program_header->p_vaddr);
        if(notes != NULL) {
          uint32_t offset = 0;
          uint8_t version = 0;
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

  Log(OGRT_LOG_DBG, "[D] sizeof(OGRT__SharedObject)=%ld\n", sizeof(OGRT__SharedObject));
  so_infos *infos = malloc(sizeof(OGRT__SharedObject) * so_count + sizeof(so_infos));
  infos->size = so_count;
  infos->index = 0;
  dl_iterate_phdr(handle_program_header, infos);

  return infos;
}
