#include "ogrt-main.h"
#include "ogrt-log.h"

/** macro for function hooking. shamelessly stolen from snoopy */
#define FN(ptr, type, name, args)  ptr = (type (*)args)dlsym(RTLD_NEXT, name)

/** global variables */
static bool  __ogrt_active   =  0;
static int   __daemon_socket = -1;
static pid_t __pid           =  0;
static pid_t __parent_pid    =  0;
static char  __hostname[HOST_NAME_MAX];

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
    ogrt_get_loaded_so();
    __ogrt_active = false;
    return 1;

    /* establish a connection the the ogrt server */
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int ret;
    if ((ret = getaddrinfo(OGRT_NET_HOST, OGRT_NET_PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "OGRT: INITIALIZE: getaddrinfo: %s\n", gai_strerror(ret));
      __ogrt_active = false;
      return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((__daemon_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("OGRT: socket");
            __ogrt_active = false;
            continue;
        }

        if (connect(__daemon_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(__daemon_socket);
            __ogrt_active = false;
            perror("OGRT: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "OGRT: INITIALIZE: failed to connect.\n");
        __ogrt_active = false;
        return 1;
    }

    freeaddrinfo(servinfo);

    fprintf(stderr, "OGRT: Connected to socket.\n");

    /* cache PID of current process - we are reusing that quite often */
    __pid = getpid();
    __parent_pid = getppid();
    if(gethostname(__hostname, HOST_NAME_MAX) != 0) {
      fprintf(stderr, "OGRT: Failed to get hostname\n");
      __ogrt_active = false;
      return 1;
    }

    fprintf(stderr, "OGRT: I be watchin' yo! (process %d with parent %d)\n", __pid, getppid());
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
    fprintf(stderr, "OGRT: I be execve: calling %s from %d\n", filename, __pid);

    /* Initialize the protobuf message, fill it, pack it and send it to the daemon */
    OGRT__Execve msg;
    ogrt__execve__init(&msg);
    msg.hostname = strdup(__hostname);
    msg.pid = __pid;
    msg.parent_pid = __parent_pid;
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
    void *msg_serialized = NULL, *msg_buffer = NULL;
    int send_length = ogrt_prepare_sendbuffer(OGRT__MESSAGE_TYPE__ExecveMsg, msg_len, &msg_buffer, &msg_serialized);

    ogrt__execve__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_buffer, send_length, 0);

    free(msg.filename);
    free(msg_buffer);
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
    fprintf(stderr, "OGRT: I be fork: spawned %d from %d\n", ret, __pid);

    /* Initialize the protobuf message, fill it, pack it and send it to the daemon */
    OGRT__Fork msg;
    ogrt__fork__init(&msg);
    msg.hostname = strdup(__hostname);
    msg.parent_pid = __pid;
    msg.child_pid = ret;

    size_t msg_len = ogrt__fork__get_packed_size(&msg);
    void *msg_serialized = NULL, *msg_buffer = NULL;
    int send_length = ogrt_prepare_sendbuffer(OGRT__MESSAGE_TYPE__ForkMsg, msg_len, &msg_buffer, &msg_serialized);

    ogrt__fork__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_buffer, send_length, 0);
    free(msg_buffer);
  }

  /* return output of original function */
  return ret;
}

/**
 * Prepare send buffer for shipping to daemon.
 * Takes a message type and the length of the payload and return the beginning of
 * the buffer, the beginning of the payload and the total size of the buffer.
 * Message format:
 *       32bit            32bit                  up to 32bit length
 * +----------------+----------------+--------------------------------------------+
 * |  message type  | payload_length |                payload                     |
 * +----------------+----------------+--------------------------------------------+
 *
 * This function is incredibly ugly. Should be reworked, but it works, right?
 */
int ogrt_prepare_sendbuffer(const int message_type, const int payload_length, void **buffer, void **payload) {
  uint32_t type = htonl(message_type);
  uint32_t length = htonl(payload_length);
  int total_length = payload_length + sizeof(type) + sizeof(length);

  *buffer = malloc(total_length);
  *payload = (((char *)*buffer) + sizeof(type) + sizeof(length));

  memcpy(*buffer, &type, sizeof(type));
  memcpy(*buffer + 4, &length, sizeof(length));

  return total_length;
}

