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
#include "ogrt.pb-c.h"

#define FN(ptr, type, name, args)  ptr = (type (*)args)dlsym(RTLD_NEXT, name)

#define SOCKET_PATH "/tmp/ogrt.sock"

static bool __ogrt_active = 0;
static int __daemon_socket = -1;

//void *_dl_sym(void *, const char *, void *);
//void *dlsym(void *handle, const char *name)
//{
//    static void * (*real_dlsym)(void *, const char *)=NULL;
//    if (real_dlsym == NULL){
//        real_dlsym=_dl_sym(RTLD_NEXT, "dlsym", dlsym);
//    }
//    /* my target binary is even asking for dlsym() via dlsym()... */
//    if (!strcmp(name,"dlsym"))
//        return (void*)dlsym;
//    printf("I be dlysm: %s from %d \n", name, getpid());
//    return real_dlsym(handle,name);
//}


/**
 * Initialize preload library.
 * Checks if tracing is activated using the environment variable OG_ASSERT_DOMINANCE.
 * If it is: connect to the unix socket of the daemon. If establishing a connection to
 * the daemon fails the init function will return with a non-zero exit code, but program
 * execution will continue as normal.
 */
__attribute__((constructor))
static int init()
{
  char *env_ogrt_active = getenv("OG_ASSERT_DOMINANCE");
  if(env_ogrt_active != NULL && (strcmp(env_ogrt_active, "yes") == 0 || strcmp(env_ogrt_active, "true") == 0 || strcmp(env_ogrt_active, "1") == 0)) {
    __ogrt_active = true;
  }
  if(__ogrt_active) {
    __daemon_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(__daemon_socket < 0) {
      perror("socket");
      __ogrt_active = 0;
      return 1;
    }
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCKET_PATH);
    if(connect(__daemon_socket, (struct sockaddr *)&remote, sizeof(remote.sun_family) + strlen(remote.sun_path)) < 0) {
      fprintf(stderr, "OGRT_INITIALIZE: ");
      perror("connect");
      __ogrt_active = 0;
      return 1;
    }
    fprintf(stderr, "OG be watchin' yo!\n");
  }
  return 0;
}

/**
 * Hook execve function.
 * This function intercepts the execve call. If OGRT is active it will send
 * the arguments of the function and the PID of the current process to the OGRT daemon.
 */
int execve(const char *filename, char *const argv[], char *const envp[]){
  static int (*unhooked_execve)(const char *, char **, char **) = NULL;
  if(unhooked_execve == NULL) { /* Cache the function address */
    FN(unhooked_execve, int, "execve", (const char *, char **const, char **const));
  }
  if(__ogrt_active) {
    fprintf(stderr, "I be execve: calling %s from %d\n", filename, getpid());

    /* Initialize the protobuf message, fill it, pack it and send it to the daemon */
    OGRT__Execve msg = OGRT__EXECVE__INIT;
    msg.pid = getpid();
    msg.filename = strdup(filename);
    int envvar_count = 0, sum = 0;
    for(char **iterator = (char **)envp; *iterator != NULL; iterator++){
      //printf("%s\n", *iterator);
      sum += strlen(*iterator)+1;
      envvar_count++;
    }
    printf("envvar = %d\n", envvar_count);
    msg.n_environment_variable = envvar_count;
    msg.environment_variable = (char **)malloc(sum);
    memcpy((&msg)->environment_variable, envp, sum);

    unsigned int msg_len = ogrt__execve__get_packed_size(&msg);
    printf("msg_size = %d\n", msg_len);
    void *msg_serialized = malloc(ogrt__execve__get_packed_size(&msg));
    ogrt__execve__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_serialized, msg_len, 0);
    free(msg.filename);
    free(msg.environment_variable);
    free(msg_serialized);

  }

  /* Call the original function and return its output */
  return (*unhooked_execve) (filename, (char**) argv, (char **) envp);
}

int execv(const char *filename, char *const argv[]) {
	static int (*unhooked_execv)(const char *, char **) = NULL;
  if(unhooked_execv == NULL) {
    FN(unhooked_execv, int, "execv", (const char *, char **const));
  }
	printf("I be execv: calling %s from %d\n", filename, getpid());
	return (*unhooked_execv) (filename, (char **) argv);
}
