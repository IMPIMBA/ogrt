#!/usr/bin/env bash
export GOPATH=$(pwd):$GOPATH
go run src/ogrt-server.go

