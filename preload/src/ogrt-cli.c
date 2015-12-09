#include "ogrt-main.h"


/**
 * Function used when executing the shared library as executable.
 * Display various information on how the program was compiled.
 */
int main(int argc, char *argv[]) {
  struct gengetopt_args_info ai;
  if (cmdline_parser(argc, argv, &ai) != 0) {
      exit(1);
  }

  if(argc == 1) {
    cmdline_parser_print_help();
  }

  if(ai.generate_signature_given) {
    char hostname[HOST_NAME_MAX+1];
    if(gethostname(hostname, sizeof(hostname)) != 0) {
      fprintf(stderr, "failed to get hostname\n");
      return 1;
    }
    printf("Host name: %s\n", hostname);

    struct passwd *pwd_entry = getpwuid(getuid());
    printf("User name: %s\n", pwd_entry->pw_name);

    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse_lower(uuid, template_signature+0x51);
    printf("UUID: %s\n", template_signature+0x51);

    for(unsigned int i=0; i < template_signature_len; i++) {
      fprintf(stderr, "%c", template_signature[i]);
    }
  }

}
