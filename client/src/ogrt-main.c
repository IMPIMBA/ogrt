#include "ogrt-main.h"

/** macro for function hooking. shamelessly stolen from snoopy */
#define FN(ptr, type, name, args)  ptr = (type (*)args)dlsym(RTLD_NEXT, name)

/** global variables */
static bool  __ogrt_active   =  0;
static int   __daemon_socket = -1;
static pid_t __pid           =  0;
static pid_t __parent_pid    =  0;
static char  __hostname[HOST_NAME_MAX+1];


FILE *ogrt_log_file;
int ogrt_log_level;
/**
 * Initialize preload library.
 * Checks if tracing is activated using the environment variable OGRT_ACTIVE.
 * If it is: connect to the unix socket of the daemon. If establishing a connection to
 * the daemon fails the init function will return with a non-zero exit code, but program
 * execution will continue as normal.
 */
__attribute__((constructor)) int ogrt_preload_init_hook()
{
  ogrt_log_file = stderr;
  ogrt_log_level = OGRT_LOG_INFO;

  if(ogrt_env_enabled("OGRT_SCHLEICHFAHRT")) {
    ogrt_log_level = OGRT_LOG_NOTHING;
  }

  if(ogrt_env_enabled("OGRT_DEBUG_INFO")) {
    cmdline_parser_print_version();
    printf("  OGRT_NET_HOST=%s\n  OGRT_NET_PORT=%s\n  OGRT_ENV_JOBID=%s\n  OGRT_ELF_SECTION_NAME=%s\n  OGRT_ELF_NOTE_TYPE=0x%x\n",
              OGRT_NET_HOST,      OGRT_NET_PORT,      OGRT_ENV_JOBID,      OGRT_ELF_SECTION_NAME,      OGRT_ELF_NOTE_TYPE);
  }

  if(ogrt_env_enabled("OGRT_ACTIVE")) {
    __ogrt_active = true;
  }

  if(__ogrt_active && __daemon_socket < 0) {
    /* establish a connection the the ogrt server */
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int ret;
    if ((ret = getaddrinfo(OGRT_NET_HOST, OGRT_NET_PORT, &hints, &servinfo)) != 0) {
      Log(OGRT_LOG_ERR, "getaddrinfo: %s\n", gai_strerror(ret));
      __ogrt_active = false;
      return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((__daemon_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            Log(OGRT_LOG_ERR, "%s\n", strerror(errno));
            __ogrt_active = false;
            continue;
        }

        if (connect(__daemon_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(__daemon_socket);
            __ogrt_active = false;
            Log(OGRT_LOG_ERR, "%s\n", strerror(errno));
            continue;
        }

        break;
    }

    if (p == NULL) {
        Log(OGRT_LOG_ERR, "%s\n", strerror(errno));
        __ogrt_active = false;
        return 1;
    }

    freeaddrinfo(servinfo);

    Log(OGRT_LOG_INFO, "Connected to socket.\n");

    /* cache PID of current process - we are reusing that quite often */
    __pid = getpid();
    __parent_pid = getppid();
    if(gethostname(__hostname, HOST_NAME_MAX) != 0) {
      Log(OGRT_LOG_ERR, "%s\n", strerror(errno));
      __ogrt_active = false;
      return 1;
    }

    Log(OGRT_LOG_INFO, "I be watchin' yo! (process %d [%s] with parent %d)\n", __pid, ogrt_get_binpath(__pid), getppid());

    if(!ogrt_send_processinfo()) {
      Log(OGRT_LOG_ERR, "failed to send process info\n");
      __ogrt_active = 0;
      return 1;
    }
  }

  return 0;
}

bool ogrt_send_processinfo() {
    //TODO: refactor the process.
    // it is kind of dirty. the currently running binary is not an so.

    so_infos *so_infos = ogrt_get_loaded_so();
    OGRT__SharedObject *shared_object_excl_blank = &(so_infos->shared_objects[2]);

    //fprintf(stderr, "OGRT: Listing shared objects:\n");

    OGRT__SharedObject *shared_object_ptr[so_infos->size];
    //for(int i = 0; i < *so_info_size; i++) {
    //  ogrt_log_debug("[D] shared object path=%s, signature=%s\n", shared_object[i].path, shared_object[i].signature);
    //  fprintf(stderr, "OGRT:\tshared object path=%s, signature=%s\n", shared_object[i].path, shared_object[i].signature);
    //  shared_object_ptr[i] = &(shared_object[i]);
    //}
    for(int i = 0; i < so_infos->size-2; i++) {
      shared_object_ptr[i] = &(shared_object_excl_blank[i]);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    OGRT__ProcessInfo msg;
    ogrt__process_info__init(&msg);
    msg.binpath = ogrt_get_binpath(__pid);
    msg.pid = __pid;
    msg.parent_pid = __parent_pid;
    msg.time = ts.tv_sec;
    char *job_id = getenv(OGRT_ENV_JOBID);
    msg.job_id = job_id == NULL ? "UNKNOWN" : job_id;
#if OGRT_MSG_SEND_USERNAME == 1
    char *username = ogrt_get_username();
    msg.username = username == NULL ? "UNKNOWN" : username;
#endif
#if OGRT_MSG_SEND_HOSTNAME == 1
    char *hostname= ogrt_get_hostname();
    msg.hostname = hostname == NULL ? "UNKNOWN" : hostname;
#endif
#if OGRT_MSG_SEND_ENVIRONMENT == 1
    #ifdef OGRT_MSG_SEND_ENVIRONMENT_WHITELIST
    size_t envvar_count = 0;
    char *environment[OGRT_MSG_SEND_ENVIRONMENT_WHITELIST_LENGTH+1];
    char *whitelist[] = { OGRT_MSG_SEND_ENVIRONMENT_WHITELIST };

    for(int i=0; i < OGRT_MSG_SEND_ENVIRONMENT_WHITELIST_LENGTH; i++) {
      Log(OGRT_LOG_DBG, "[D] checking env variable: %s\n", whitelist[i]);
      char *env = getenv(whitelist[i]);
      if(env != NULL) {
        Log(OGRT_LOG_DBG, "[D] storing : %s with value '%s'\n", whitelist[i], env);
        int ret = asprintf(&(environment[envvar_count++]), "%s=%s", whitelist[i], env);
        if(ret == -1) {
          Log(OGRT_LOG_ERR, "failed copying environment variable\n");
        }
      }
    }
    #else
    size_t envvar_count = 0;
    char **environment = environ;
    for(char **iterator = environment; *iterator != NULL; iterator++){
      envvar_count++;
    }
    #endif
    msg.n_environment_variables = envvar_count;
    msg.environment_variables = environment;
#endif
    if(so_infos->shared_objects[0].signature != NULL) {
      msg.signature = so_infos->shared_objects[0].signature;
    }
    msg.n_shared_objects = so_infos->size-2;
    msg.shared_objects = shared_object_ptr;

    size_t msg_len = ogrt__process_info__get_packed_size(&msg);
    void *msg_serialized = NULL;
    char *msg_buffer = NULL;
    int send_length = ogrt_prepare_sendbuffer(OGRT__MESSAGE_TYPE__ProcessInfoMsg, msg_len, &msg_buffer, &msg_serialized);

    ogrt__process_info__pack(&msg, msg_serialized);
    send(__daemon_socket, msg_buffer, send_length, 0);

    /* free stuff */
    free(msg.binpath);
    for(int i=0; i < so_infos->size; i++) {
      free(so_infos->shared_objects[i].path);
    }
    free(so_infos);
    free(msg_buffer);
#if OGRT_MSG_SEND_USERNAME == 1
    free(username);
#endif
#if OGRT_MSG_SEND_HOSTNAME == 1
    free(hostname);
#endif
#if OGRT_MSG_SEND_ENVIRONMENT == 1
#ifdef OGRT_MSG_SEND_ENVIRONMENT_WHITELIST
    for(int i=0; i < OGRT_MSG_SEND_ENVIRONMENT_WHITELIST_LENGTH; i++) {
      free(environment[i]);
    }
#endif
#endif
    return true;
}

#if 0
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
#endif

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
int ogrt_prepare_sendbuffer(const int message_type, const int payload_length, char **buffer, void **payload) {
  uint32_t type = htonl(message_type);
  uint32_t length = htonl(payload_length);
  int total_length = payload_length + sizeof(type) + sizeof(length);

  *buffer = malloc(total_length);
  *payload = (((char *)*buffer) + sizeof(type) + sizeof(length));

  memcpy(*buffer, &type, sizeof(type));
  memcpy(*buffer + 4, &length, sizeof(length));

  return total_length;
}

