#include "ogrt-util.h"

/** Environment Functions **/

bool ogrt_env_enabled(char *env_variable) {
  char *env_var = getenv(env_variable);
  if(env_var != NULL && (strcmp(env_var, "yes") == 0 || strcmp(env_var, "true") == 0 || strcmp(env_var, "1") == 0)) {
    return true;
  }
  return false;
}

/** File Functions **/
/**
 * Normalize a path to not contain '..' or '.'.
 * TODO: This function uses the glibc realpath internally and could do disk access.
 */
char *ogrt_normalize_path(const char *path) {
  char *normalized_path = malloc(PATH_MAX);
  char *ret = realpath(path, normalized_path);
  if(ret == NULL) {
    perror("realpath");
    free(normalized_path);
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
    free(bin_path);
    return NULL;
  }

  ssize_t len = readlink(proc_path, bin_path, PATH_MAX);
  if (len == -1) {
     perror("lstat");
     free(bin_path);
     return NULL;
  }

  bin_path[len] = '\0';
  return bin_path;
}

