#ifndef OGRT_H_INCLUDED
#define OGRT_H_INCLUDED

#include "ogrt.pb-c.h"

/** network configuration */
#define OGRT_NET_HOST     ("localhost")
#define OGRT_NET_PORT     ("6074")

/** ELF stamping configuration */
#define OGRT_SECTION_NAME (".note.ogrt.info")

/** function prototypes */
int ogrt_read_info(const char *filename);
int ogrt_prepare_sendbuffer(const int message_type, const int message_length, void **buffer_begin, void **payload);

#endif
