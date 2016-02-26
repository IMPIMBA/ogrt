#include "ogrt-test.h"

#include "../src/ogrt-main.h"

TEST_INIT

TEST_START(ogrt_send_processinfo, "run processinfo")
{
  ogrt_send_processinfo();
  TEST_PASS;
}
TEST_END

