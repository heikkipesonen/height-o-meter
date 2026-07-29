#!/bin/bash
# Usage: ./change_id.sh CURRENT_ID NEW_ID
# Example: ./change_id.sh 80 81

CURRENT_ID=$1
NEW_ID=$2
DEV="/dev/ttyUSB0"

# Convert IDs to hex (1 byte)
CURRENT_ID_HEX=$(printf "%02X" $CURRENT_ID)
NEW_ID_HEX=$(printf "%02X" $NEW_ID)

# Function to calculate CRC16-Modbus
crc16_modbus() {
  local data=$1
  local crc=0xFFFF
  local polynomial=0x8005
  local i j

  # Convert hex string to array of bytes
  local -a bytes=()
  for ((i = 0; i < ${#data}; i += 2)); do
    bytes+=("0x${data:$i:2}")
  done

  # Calculate CRC
  for byte in "${bytes[@]}"; do
    crc=$((crc ^ byte))
    for ((j = 0; j < 8; j++)); do
      if ((crc & 0x0001)); then
        crc=$(((crc >> 1) ^ polynomial))
      else
        crc=$((crc >> 1))
      fi
    done
  done

  # Return CRC as two bytes (high and low)
  printf "%02X %02X" $(((crc >> 8) & 0xFF)) $((crc & 0xFF))
}

# Function to send Modbus frame (with dynamic CRC)
send_frame() {
  local frame_hex=$1
  local crc=$(crc16_modbus "$frame_hex")
  echo -ne "\\x${frame_hex// /}\\x${crc// /}" >$DEV
  sleep 0.1
}

# 1. Unlock (write KEY=0x65 to register 0x00)
send_frame "$CURRENT_ID_HEX 06 00 00 00 65"

# 2. Write new ID to register 0x01 (Slave Address)
FRAME_HEX="$CURRENT_ID_HEX 06 00 01 00 $NEW_ID_HEX"
send_frame "$FRAME_HEX"

# 3. Save (write SAVE=0x01 to register 0x00)
send_frame "$CURRENT_ID_HEX 06 00 00 00 01"

echo "Sensor ID changed from $CURRENT_ID to $NEW_ID. Power cycle the sensor."
