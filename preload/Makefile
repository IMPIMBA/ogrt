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
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

# libraries
STATIC_LIBS=-lprotobuf-c -luuid
SHARED_LIBS=-ldl -lrt #TODO: lrt is only necessary < glibc 2.17 (http://stackoverflow.com/questions/2418157/ubuntu-linux-c-error-undefined-reference-to-clock-gettime-and-clock-settim)

# compiler/linker flags
CC=gcc
CFLAGS=-fPIC -Wall -Wextra -std=c11 -g -O3
LDFLAGS=-pie -Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic $(SHARED_LIBS)

# directories
SRCDIR=src
BUILDDIR=build
OUTDIR=lib

# files
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
STAMP=$(SRCDIR)/stamp.s
TARGET=ogrt

# utility functions
dir_guard=@mkdir -p $(@D)

# targets
all: $(OUTDIR)/$(TARGET)

#TODO: this does not work right now
$(SRCDIR)/ogrt-cmdline.c: $(SRCDIR)/ogrt-cmdline.ggo
	gengetopt --output-dir $(SRCDIR) --set-version $(GIT_VERSION) --input $(SRCDIR)/ogrt-cmdline.ggo --include-getopt -F ogrt-cmdline

$(OUTDIR)/$(TARGET): $(OBJECTS)
	$(dir_guard)
	@sed -i s/@@UUID@@/$(shell uuidgen)/ $(STAMP)
	$(CC) $(OBJECTS) $(STAMP) -o $@ $(LDFLAGS)

$(OBJECTS): $(BUILDDIR)/%.o : $(SRCDIR)/%.c
	$(dir_guard)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean:
	-rm -rf $(BUILDDIR) $(OUTDIR)
