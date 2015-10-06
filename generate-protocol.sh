#!/usr/bin/env bash

set -euo pipefail

protoc --go_out ogrt-server/ ogrt-protocol/ogrt.proto

protoc-c --c_out ogrt-preload/src/ ogrt-protocol/ogrt.proto
mv ogrt-preload/src/ogrt-protocol/* ogrt-preload/src/
rmdir  ogrt-preload/src/ogrt-protocol
sed -i 's|#include "ogrt-protocol/ogrt.pb-c.h"|#include "ogrt.pb-c.h"|' ogrt-preload/src/ogrt.pb-c.c
