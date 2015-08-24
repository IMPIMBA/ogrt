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
#include "ogrt.h"
#include "ogrt.pb-c.h"

/** macro for function hooking. shamelessly stolen from snoopy */
#define FN(ptr, type, name, args)  ptr = (type (*)args)dlsym(RTLD_NEXT, name)

#define SOCKET_PATH "/tmp/ogrt.sock"

/* global variables */
static bool  __ogrt_active   = 0;
static int   __daemon_socket = -1;
static pid_t __pid           = 0;

/**
 * Initialize preload library.
 * Checks if tracing is activated using the environment variable OG_ASSERT_DOMINANCE.
 * If it is: connect to the unix socket of the daemon. If establishing a connection to
 * the daemon fails the init function will return with a non-zero exit code, but program
 * execution will continue as normal.
 */
__attribute__((constructor)) static int init()
{
  char *env_ogrt_active = getenv("OG_ASSERT_DOMINANCE");
  if(env_ogrt_active != NULL && (strcmp(env_ogrt_active, "yes") == 0 || strcmp(env_ogrt_active, "true") == 0 || strcmp(env_ogrt_active, "1") == 0)) {
    __ogrt_active = true;
  }
  if(__ogrt_active && __daemon_socket < 0) {
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
    /* cache PID of current process - we are reusing that quite often */
    __pid = getpid();

    fprintf(stderr, "OG be watchin' yo! (process %d with parent %d)\n", __pid, getppid());
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
    fprintf(stderr, "I be execve: calling %s from %d\n", filename, __pid);

    /* Initialize the protobuf message, fill it, pack it and send it to the daemon */
    OGRT__Execve msg;
    ogrt__execve__init(&msg);
    msg.pid = __pid;
    msg.filename = strdup(filename);

    /* count number of environment variables and pass to message */
    size_t envvar_count = 0;
    for(char **iterator = (char **)envp; *iterator != NULL; iterator++){
      envvar_count++;
    }
    msg.n_environment_variables = envvar_count;
    msg.environment_variables = (char **)envp;
    /* count number of arguments and pass to message */
    size_t argv_count = 0;
    for(char **iterator = (char **)argv; *iterator != NULL; iterator++){
      argv_count++;
    }
    msg.n_arguments = argv_count;
    msg.arguments = (char **)argv;

    size_t msg_len = ogrt__execve__get_packed_size(&msg);
    void *msg_serialized = malloc(msg_len);
    ogrt__execve__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_serialized, msg_len, 0);

    free(msg.filename);
    free(msg_serialized);
  }

  /* Call the original function and return its output */
  return (*unhooked_execve) (filename, (char**) argv, (char **) envp);
}

/**
 * Hook fork function.
 * This function intercepts the fork call. If OGRT is active it will send
 * the PID of the forked function and the PID of the current process to the OGRT daemon.
 */
int fork(void){
  static int (*unhooked_fork)() = NULL;
  if(unhooked_fork == NULL) { /* Cache the function address */
    FN(unhooked_fork, int, "fork", (void));
  }

  int ret = (*unhooked_fork)();
  /* ignore parent process */
  if(__ogrt_active && ret != 0) {
    fprintf(stderr, "I be fork: spawned %d from %d\n", ret, __pid);

    /* Initialize the protobuf message, fill it, pack it and send it to the daemon */
    OGRT__Fork msg;
    ogrt__fork__init(&msg);
    msg.parent_pid = __pid;
    msg.child_pid = ret;

    size_t msg_len = ogrt__fork__get_packed_size(&msg);
    void *msg_serialized = malloc(msg_len);
    ogrt__fork__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_serialized, msg_len, 0);
    free(msg_serialized);
  }

  /* return output of original function */
  return ret;
}
