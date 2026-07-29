#!/bin/bash
# Install dependencies for Raspberry Pi OS / Debian / Ubuntu

set -e

echo "Installing dependencies..."
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libmodbus-dev

echo "Done."
