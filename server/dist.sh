#!/usr/bin/env bash

set -eou pipefail

./build.sh

VERSION="$(git describe --dirty --always --tags --abbrev=6)"
DIST="ogrt-server-${VERSION}"
mkdir -p "dist/$DIST"
cd dist
cp ../ogrt-server "$DIST"
cp ../ogrt.conf "$DIST"

tar cjf "${DIST}.tar.bz2" "$DIST"
rm -rf "${DIST}"
