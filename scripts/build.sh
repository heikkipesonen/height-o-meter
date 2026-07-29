#!/bin/bash
# Build the project

set -e

cd "$(dirname "$0")/.."

mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo ""
echo "Build complete: build/heightmatic"
