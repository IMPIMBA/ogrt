#include "ogrt-util.h"

/**
 * Check if an environment variable is true
 */
bool ogrt_env_enabled(char *env_variable) {
  char *env_var = getenv(env_variable);
  if(env_var != NULL && (strcmp(env_var, "yes") == 0 || strcmp(env_var, "true") == 0 || strcmp(env_var, "1") == 0)) {
    return true;
  }
  return false;
}

/**
 * Normalize a path to not contain '..' or '.'.
 * TODO: This function uses the glibc realpath internally and could do disk access.
 */
char *ogrt_normalize_path(const char *path) {
  char *normalized_path = malloc(PATH_MAX);
  if (normalized_path == NULL) {
    fprintf(stderr, "OGRT: memory allocate failed\n");
    free(normalized_path);
    return NULL;
  }
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

/**
 * Given the pid of a program return the cmdline.
 * This is done by walking /proc.
 */
char *ogrt_get_cmdline(const pid_t pid) {
  char proc_path[PATH_MAX];
  sprintf(proc_path, "/proc/%d/cmdline", pid);

  char *cmdline;
  cmdline = malloc(PATH_MAX);
  if (cmdline == NULL) {
    fprintf(stderr, "OGRT: memory allocate failed\n");
    return NULL;
  }

  int fd = open(proc_path, O_RDONLY);
  int nbytesread = read(fd,cmdline,PATH_MAX);
  fprintf(stderr, "OGRT: read cmdline bytes: %d\n",nbytesread);
  char *end = cmdline + nbytesread;
  for(char * p=cmdline; p<end;){
	while(*p++);
	*(p-1) = ' ';
  }
  *end='\0';
  
  return cmdline;
}

/**
 * Get username of the current user.
 * Does not use environment variables to do the lookup.
 */
char *ogrt_get_username(){
  struct passwd *pw = getpwuid(geteuid());
  if(pw == NULL) {
    return NULL;
  }
  return strdup(pw->pw_name);
}

/**
 * Get the name of the current host.
 */
char *ogrt_get_hostname(){
  char hostname[HOST_NAME_MAX+1];
  int ret = gethostname(hostname, sizeof(hostname));
  if(ret) {
    return NULL;
  }
  return strdup(hostname);
}

