#!/usr/bin/env bash
## Script to generate protocol definitions
set -euo pipefail

function is_available() {
  command -v $1 >/dev/null 2>&1 || { echo >&2 "$1 not available."; exit 1; }
}

is_available "protoc"
is_available "protoc-c"

protoc --go_out ogrt-server/ ogrt-protocol/ogrt.proto
protoc-c --c_out ogrt-preload/src/ ogrt-protocol/ogrt.proto

# copy C source into right location
mv ogrt-preload/src/ogrt-protocol/* ogrt-preload/src/
rmdir  ogrt-preload/src/ogrt-protocol
sed -i 's|#include "ogrt-protocol/ogrt.pb-c.h"|#include "ogrt.pb-c.h"|' ogrt-preload/src/ogrt.pb-c.c
