#!/usr/bin/env bash
## Stamp an ELF file with OGRT signature
set -euo pipefail

file=${1:-}

STAMP_FILE=$(mktemp)
STAMP_VERSION="00"
UUID=$(uuidgen -r)
echo -ne "$STAMP_VERSION\\00$UUID\\00" > "$STAMP_FILE"
objcopy --add-section ".note.ogrt.info=$STAMP_FILE" "$file" "$(basename "$file").stamped"
echo "Wrote $(basename "$file").stamped with OGRT signature."
