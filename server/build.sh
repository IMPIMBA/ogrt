#!/usr/bin/env bash
export GOPATH=$(pwd):$GOPATH
# build a static binary
export CGO_ENABLED=0

go build -ldflags "-X main.VERSION=$(git describe --dirty --always --tags --abbrev=6)" src/ogrt-server.go
