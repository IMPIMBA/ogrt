#include "ogrt-log.h"

#ifdef OGRT_DEBUG
void ogrt_log_debug(const char *msg, ...) {
  va_list args;
  va_start(args, msg);

  vfprintf(stderr, msg, args);

  va_end(args);
}
#else
void ogrt_log_debug(__attribute__((unused)) const char *msg, ...) {
}
#endif
