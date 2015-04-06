#include <unistd.h>

void main() {
  char *argv[] = { "ps", NULL };
  char *envp[] = { "YO=SD", NULL };

  execve("/bin/ps", argv, envp);

}
