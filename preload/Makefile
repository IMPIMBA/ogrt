##
# Makefile for OGRT preload library
##
# Build the ogrt_preload.so library.
# Try to link in dependencies statically for easier deployment.
#
# Dependencies:
# 	libprotobuf-c
###

# versioning
GIT_VERSION := $(shell git describe --abbrev=4 --always --tags)

# libraries
STATIC_LIBS=-lprotobuf-c -luuid
SHARED_LIBS=-ldl -lrt #TODO: lrt is only necessary < glibc 2.17 (http://stackoverflow.com/questions/2418157/ubuntu-linux-c-error-undefined-reference-to-clock-gettime-and-clock-settim)

# compiler/linker flags
CC=gcc
CFLAGS=-fPIC -Wall -Wextra -std=c11 -g -O3
LIB_LDFLAGS=-shared -Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic $(SHARED_LIBS)
CLI_LDFLAGS=-Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic $(SHARED_LIBS)

# directories
SRCDIR=src
STAMP=$(SRCDIR)/stamp.s

CLI_SOURCES=$(SRCDIR)/ogrt-cli.c $(SRCDIR)/ogrt-cli.h
CLI_TARGET=ogrt-cli
CLI_OUT=bin

LIB_SOURCES=$(filter-out $(CLI_SOURCES), $(wildcard $(SRCDIR)/*.c))
LIB_TARGET=ogrt.so
LIB_OUT=lib

# utility functions
dir_guard=@mkdir -p $(@D)

# targets
all: $(LIB_OUT)/$(LIB_TARGET) $(CLI_OUT)/$(CLI_TARGET)

.PHONY: force
git_version: force
	@echo '$(GIT_VERSION)' | cmp -s - $@ || echo '$(GIT_VERSION)' > $@

$(SRCDIR)/ogrt-cmdline.c: $(SRCDIR)/ogrt-cmdline.ggo git_version
	gengetopt --output-dir $(SRCDIR) --set-version $(GIT_VERSION) --input $(SRCDIR)/ogrt-cmdline.ggo --include-getopt -F ogrt-cmdline


$(LIB_OUT)/$(LIB_TARGET): $(LIB_SOURCES)
	$(dir_guard)
	@sed -i s/@@UUID@@/$(shell uuidgen)/ $(STAMP)
	$(CC) $(STAMP) -o $@  $^ $(CFLAGS) $(LIB_LDFLAGS)


$(CLI_OUT)/$(CLI_TARGET): $(CLI_SOURCES) $(LIB_SOURCES)
	$(dir_guard)
	@sed -i s/@@UUID@@/$(shell uuidgen)/ $(STAMP)
	$(CC) $(STAMP) -o $@  $^ $(CFLAGS) $(CLI_LDFLAGS)

.PHONY : clean
clean:
	-rm -rf $(CLI_OUT) $(LIB_OUT)
