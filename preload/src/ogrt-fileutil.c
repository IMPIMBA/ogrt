#include "ogrt-fileutil.h"


/**
 * Normalize a path to not contain '..' or '.'.
 * TODO: This function uses the glibc realpath internally and could do disk access.
 */
char *ogrt_normalize_path(const char *path) {
  char *normalized_path = malloc(PATH_MAX);
  char *ret = realpath(path, normalized_path);
  if(ret == NULL) {
    perror("realpath");
    return NULL;
  }
  return normalized_path;
}

/**
 * Given the pid of a program return the path to its binary.
 * This is done by walking /proc.
 */
char *ogrt_get_binpath(const pid_t pid) {
  char proc_path[PATH_MAX];
  sprintf(proc_path, "/proc/%d/exe", pid);

  char *bin_path;
  bin_path = malloc(PATH_MAX);
  if (bin_path == NULL) {
    fprintf(stderr, "OGRT: memory allocate failed\n");
    return NULL;
  }

  ssize_t len = readlink(proc_path, bin_path, PATH_MAX);
  if (len == -1) {
     perror("lstat");
     return NULL;
  }

  bin_path[len] = '\0';
  return bin_path;
}

