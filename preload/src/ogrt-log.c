#include "ogrt-log.h"

#define LOG_FATAL    (1)
#define LOG_ERR      (2)
#define LOG_WARN     (3)
#define LOG_INFO     (4)
#define LOG_DBG      (5)

#define Log(level, ...) do {  \
                              if (level <= debug_level) { \
                                                                fprintf(dbgstream,"%s:%d:", __FILE__, __LINE__); \
                                                                fprintf(dbgstream, __VA_ARGS__); \
                                                                fprintf(dbgstream, "\n"); \
                                                                fflush(dbgstream); \
                                                            } \
                          } while (0)

extern FILE *dbgstream;
extern int  debug_level;

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
