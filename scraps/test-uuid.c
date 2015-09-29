#include <uuid/uuid.h>
#include <stdio.h>

void main() {
  uuid_t uuid;
  uuid_generate(uuid);
  for (size_t i = 0; i < sizeof uuid; i ++) {
      printf("%02x", uuid[i]);
  }
}
