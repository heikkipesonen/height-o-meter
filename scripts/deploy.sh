#!/bin/bash
# Deploy to Raspberry Pi and run

set -e

PI_HOST="${PI_HOST:-pi@raspberrypi.local}"
PI_PATH="${PI_PATH:-~/height-o-meter}"

echo "Syncing to $PI_HOST..."
rsync -avz --exclude 'build/' \
    "$(dirname "$0")/../" \
    "$PI_HOST:$PI_PATH/"

echo "Building and running on Pi..."
ssh -t "$PI_HOST" "cd $PI_PATH && ./scripts/build.sh && SDL_VIDEODRIVER=kmsdrm ./build/heightmatic"
