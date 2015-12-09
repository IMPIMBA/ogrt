bin_PROGRAMS = ogrt
ogrt_SOURCES = ogrt-cli.c ogrt-cmdline.c ogrt-log.c ogrt.pb-c.c ogrt-readso.c ogrt-signature.c ogrt-util.c
ogrt_CFLAGS  = $(protobufc_CFLAGS)
ogrt_LDFLAGS = -Wl,-Bstatic $(protobufc_LIBS) $(uuid_LIBS) -Wl,-Bdynamic

lib_LTLIBRARIES = ogrt.la
ogrt_la_SOURCES = ogrt-cmdline.c ogrt-log.c ogrt-main.c ogrt.pb-c.c ogrt-readso.c ogrt-signature.c ogrt-util.c
ogrt_la_CFLAGS  = $(protobufc_CFLAGS)
ogrt_la_LDFLAGS = -avoid-version -module -shared -export-dynamic -Wl,-Bstatic $(protobufc_LIBS) $(uuid_LIBS) -Wl,-Bdynamic
ogrt_la_LIBADD  = -ldl



