# OGRT

## Ominous Glibc Runtime Tracker

Tool to hook into glibc calls and track them.

## Requirements

For development:

- Google Protocol Buffers (https://github.com/google/protobuf)
- Protocol Buffers for C (https://github.com/protobuf-c/protobuf-c) compiled with PIC
- libelf

For runtime:

- libelf (can not get it to build statically at the moment)

## Project Structure

### ogrt_preload

Preload library written in C.

Hooks glibc functions (currently fork and execve), packs up information
about them into a protobuf message and sends it (over a tcp socket) to
ogrt_server. Failure in the preload library does not interrupt normal
program execution.

This library should be very light by design. The only thing it should do
is pack up arguments of the hooked functions into a protobuf and send
them over the wire.

### ogrt_server

Daemon written in Go.

Receives protobuf messages from the preload library, does some preprocessing and persists
them into a database.

### ogrt_proto

Contains the protobuf protocol definition.

### ogrt_stamp

Contains a bash script that watermarks ('stamps') ELF binaries with an OGRT
signature. This is currently a work-in-progress and should not be used.

### tests

Contains some testing programs. They are not really tests and are used
by the author. They should be replaced by real tests.

### scraps

Contains some random stuff the author has been trying out. 
