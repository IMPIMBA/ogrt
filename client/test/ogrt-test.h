#include <assert.h>

#define TEST_START(name, description) void __attribute__((constructor)) test_##name(void) {\
    printf("[T: %s] %s: ", __func__, description);
#define TEST_END printf("\n"); }

#define TEST_INIT int main() {}

#define TEST_PASS printf("[PASS]");
#define TEST_FAIL(reason) printf("%s [FAIL]", reason); exit(-1);


