#!/usr/bin/env bash
# Build the asstudio CLI (Odin).
# Usage: ./tools/build-cli.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

mkdir -p build

odin build src/app \
  -out:build/asstudio \
  -collection:src=src \
  -o:speed

echo "built: $ROOT/build/asstudio"
echo "try:   ./build/asstudio validate samples/hello.json"
echo "       ./build/asstudio generate samples/hello.json -o generated/hello"
