all:
	gcc -shared -fPIC -Wall -Wextra -std=c11 -o preload.so ogrt.c ogrt_elf.c ogrt.pb-c.c -ldl -lelf -Wl,-Bstatic -lprotobuf-c -Wl,-Bdynamic
