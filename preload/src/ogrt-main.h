#ifndef OGRT_H_INCLUDED
#define OGRT_H_INCLUDED

#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ogrt.pb-c.h"
#include "ogrt-readso.h"

/* debug settings */
#define OGRT_DEBUG

/** network configuration */
#define OGRT_NET_HOST     ("localhost")
#define OGRT_NET_PORT     ("6074")

/** ELF stamping configuration */
#define OGRT_ELF_SECTION_NAME (".note.ogrt.info")
#define OGRT_ELF_NOTE_TYPE    (0x4f475254)

/** function prototypes */
int ogrt_read_info(const char *filename);
int ogrt_read_info();
int ogrt_prepare_sendbuffer(const int message_type, const int message_length, void **buffer_begin, void **payload);

#endif
