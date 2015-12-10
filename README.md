# OGRT

## Ominous Glibc Runtime Tracker

OGRT is a tool designed to track user processes on a HPC cluster.
It is very similar to [XALT](https://github.com/Fahey-McLay/xalt) in nature.

Be warned: this is experimental code. If you deploy this into production
it will ruin your day. Also it will not work out of the box.

## Project Structure

### client

Preload library written in C. It needs to be preloaded (check 'man 8 ld.so'
for a more thorough explanation) into the process that needs to
be tracked.

This library uses GNU libc facilities to query the loaded shared objects
of the process it was loaded into. It also checks these shared objects
for a signature. This signature is not used at the moment, but it is
intended for tagging programs at link time and then reading them at
runtime.

All information gathered by this library is packed into a protobuf message
and sent (over a tcp socket) to the server. Failure in the preload library
should not interrupt normal program execution.

This library should be very light by design. The only thing it should do
is pack up arguments of the hooked functions into a protobuf and send
them over the wire. Also it accesses the filesystem as little as
possible (at the moment only for resolution of relative paths to
absolute ones - eventually).

### server

Daemon written in Go. The purpose of this daemon is receive and
preprocess data from the preload library, before persisting it.

It knows how to write JSON over TCP and to write JSON to files.
The JSON to file output is _very_ slow at the moment and is only
intended for debugging purposes.

### protocol

Contains the protobuf protocol definition. After modifying this file you
need to run 'generate-protocol' and recompile the preload library and
the daemon.

## Building

### client

Requirements:

* [Google Protocol Buffers](https://github.com/google/protobuf)
* [Protocol Buffers for C](https://github.com/protobuf-c/protobuf-c) (as static library)
* the uuid library from util-linux (also static)
* libelf development headers

Compilation:

1. Make sure your machine fulfills the requirements
2. Change to the client directory
2. Run ./configure --server-host=[ogrt-server] --server-port=7971
   --env-jobid="JOBID"
3. Run 'make install'
4. You now have libogrt.so in /usr/local/lib, which talks to
   [ogrt-server] on port 7971 and uses the environment variable JOBID to
   figure out the ID of the currently running job

### server

Requirements:

* [Google Protocol Buffers](https://github.com/google/protobuf)
* [A TOML parser](https://github.com/BurntSushi/toml)

Compilation:

1. Make sure you have a working installation of go
2. Set the GOPATH to the absolute path of the server directory
2. go get "github.com/BurntSushi/toml"
3. go get "github.com/golang/protobuf/proto"
4. Run 'go build src/ogrt-server.go' in the 'server' directory
5. Your server binary is 'server/ogrt-server'
6. For guidance on how to configure the outputs check ogrt.conf in the
   server directory.

## Running

1. Run the server. The config file should be in the same directory as
   the server and the name must be named ogrt.conf. The default ogrt.conf 
   should be enough to get started.
2. Preload the library and set the necessary environment variables:
   LD_PRELOAD=/usr/local/lib/libogrt.so OGRT_ACTIVE=1 ls
3. You should have seen some output and a JSON of the program run
    should be in /tmp/ogrt_jobs/.

### Client Environment Variables

- OGRT_ACTIVE - activate OGRT
- OGRT_SCHLEICHFAHRT - supresses all output
- OGRT_DEBUG_INFO - print the settings OGRT was compiled with


## License

All of this code is GPL3 licensed.

