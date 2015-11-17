#!/usr/bin/env bash
## Script to generate protocol definitions
set -euo pipefail

function is_available() {
  command -v $1 >/dev/null 2>&1 || { echo >&2 "$1 not available."; exit 1; }
}

is_available "protoc"
is_available "protoc-c"

protoc --go_out server/ protocol/ogrt.proto
protoc-c --c_out preload/src/ protocol/ogrt.proto

# copy C source into right location
mv preload/src/protocol/* preload/src/
rmdir  preload/src/protocol
sed -i 's|#include "protocol/ogrt.pb-c.h"|#include "ogrt.pb-c.h"|' preload/src/ogrt.pb-c.c
