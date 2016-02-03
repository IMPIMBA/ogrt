#include "ogrt-main.h"
#include "ogrt-test.h"

TEST_INIT

TEST_FUNC(ogrt_preload_init_hook, "test process info")
  ogrt_preload_init_hook();
TEST_END

