#!/usr/bin/env bash

set -eou pipefail

./build.sh

VERSION="$(git describe --dirty --always --tags --abbrev=6)"
DIST="ogrt-server-${VERSION}"
mkdir -p "dist/$DIST"
cd dist
cp ../ogrt-server "$DIST"
cp ../ogrt.conf "$DIST"

export XZ_OPT="-9"
tar cJf "${DIST}.tar.xz" "$DIST"
rm -rf "${DIST}"
