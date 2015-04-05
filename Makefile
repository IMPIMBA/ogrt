all:
	gcc -shared -fPIC -Wall -std=c99 -o preload.so preload.c ogrt.pb-c.c -ldl -Wl,--Bstatic -lprotobuf-c -Wl,-Bdynamic
