# OGRT

## Ominous Glibc Runtime Tracker

OGRT is a tool designed to track user processes on a HPC cluster.
It is very similar to [XALT](https://github.com/Fahey-McLay/xalt) in nature.

Be warned: this is experimental code. If you deploy this into production
it will ruin your day. Also it will not work out of the box.

## Project Structure

### preload

Preload library written in C. It needs to be preloaded (check 'man 8 ld.so'
for the LD_PRELOAD environment variable) into the process that needs to
be tracked.

This library uses GNU libc facilities to query the loaded shared objects
of the process it was loaded into. It also checks these shared objects
for a signature (see the signature section of this document on how to
generate such a signature).

All information gathered by this library is packed into a protobuf message
and sent (over a tcp socket) to the server. Failure in the preload library
should not interrupt normal program execution.

This library should be very light by design. The only thing it should do
is pack up arguments of the hooked functions into a protobuf and send
them over the wire. Also it accesses the filesystem as little as
possible.

### server

Daemon written in Go. The purpose of this daemon is receive and
preprocess data from the preload library, before persisting it.

This currently writes JSON information into the './jobs' directory.

### signature

Contains ogrt-stamp, which generates a GNU assembly file with an OGRT
signature. This assembly file needs to be linked into the program that
you want to watermark.

### protocol

Contains the protobuf protocol definition. After modifying this file you
need to run 'generate-protocol' and recompile the preload library and
the daemon.

## Building

### preload

Requirements:

- Google Protocol Buffers (https://github.com/google/protobuf)
- Protocol Buffers for C (https://github.com/protobuf-c/protobuf-c) compiled with static and PIC

1. Make sure your machine fulfills the requirements
2. Check 'preload/src/ogrt-main.h' for settings for the library
3. Run 'make' in the 'preload' directory
4. Your library is in 'preload/lib'

### server

1. Run 'go build ogrt-server.go' in the 'server' directory
2. Your server binary is 'server/ogrt-server'

## License

All of this code is GPL3 licensed.
