#!/bin/bash
# Install dependencies for Arch-based systems (CachyOS, Manjaro, EndeavourOS)

set -e

echo "Installing dependencies..."
sudo pacman -S --needed \
    base-devel \
    cmake \
    sdl2 \
    sdl2_ttf \
    libmodbus

echo "Done."
