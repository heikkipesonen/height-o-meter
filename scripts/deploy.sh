#!/bin/bash
# Quick deploy to Pi - sync, rebuild, restart

PI="rpi@192.168.4.200"
DIR="height-o-meter"

echo "Syncing..."
rsync -av --checksum --exclude 'build/' --exclude '.git/' ~/Documents/height-o-meter/src/ $PI:~/$DIR/src/

echo "Building..."
ssh $PI "cd ~/$DIR && make -C build -j4"

echo "Restarting..."
ssh $PI "sudo systemctl restart heightmatic"

sleep 1
echo "--- Status ---"
ssh $PI "systemctl status heightmatic --no-pager | head -8"
