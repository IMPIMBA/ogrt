#include "ogrt-test.h"

#include "../src/ogrt-util.h"
#include <stdlib.h>

TEST_INIT

TEST_START(ogrt_get_hostname, "check if hostname is not null")
{
  char *hostname = ogrt_get_hostname();
  assert(hostname != NULL);
  free(hostname);
  TEST_PASS;
}
TEST_END

TEST_START(ogrt_get_username, "check if username is not null")
{
  char *username = ogrt_get_username();
  assert(username != NULL);
  free(username);
  TEST_PASS;
}
TEST_END

TEST_START(ogrt_get_cmdline, "check if cmdline is not null")
{
  char *cmdline = ogrt_get_cmdline(getpid());
  assert(cmdline != NULL);
  free(cmdline);
  TEST_PASS;
}
TEST_END
