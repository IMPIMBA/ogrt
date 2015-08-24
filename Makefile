all:
	gcc -shared -fPIC -Wall -Wextra -std=c99 -o preload.so ogrt.c ogrt_elf.c ogrt.pb-c.c -ldl -Wl,-Bstatic -lprotobuf-c -lelf -Wl,-Bdynamic
