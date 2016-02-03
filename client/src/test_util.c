#include "ogrt-util.h"
#include "ogrt-test.h"

TEST_INIT

TEST_FUNC(ogrt_get_hostname, "check for hostname")
  printf("%s", ogrt_get_hostname());
TEST_END

TEST_FUNC(ogrt_get_username, "check for username")
  printf("%s", ogrt_get_username());
TEST_END
