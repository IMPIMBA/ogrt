#!/usr/bin/env bash
[[ -d config ]] || mkdir config
autoreconf --force --install -I config -I m4
