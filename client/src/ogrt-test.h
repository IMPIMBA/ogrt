#define TEST_FUNC(name, description) void __attribute__((constructor)) test_##name(void) { \
    printf("%s: ", description);
#define TEST_END printf("\n"); }
#define TEST_INIT int main() {}

