# Excavator grade control for broke ass people

Uses witmotion industrial angle sensors to calculate boom vertical & horizontal position in relation to center pivot point for simple grade control.

## Hardware

- Raspberry Pi with DSI touchscreen (800x480)
- Witmotion angle sensors (RS485 Modbus)
- FTDI USB-RS485 adapter

## Sensors

6 sensor positions defined in `excavator.h`:
- SENSOR_SUPERSTRUCTURE (id 1) - cab rotation
- SENSOR_BOOM_A (id 2) - main boom angle
- SENSOR_BOOM_B (id 3) - secondary boom angle  
- SENSOR_STICK (id 4) - stick/arm angle
- SENSOR_TILT (id 5) - bucket tilt
- SENSOR_TEST (id 80) - for setup/probing

Sensor IDs are configured in `main.cpp`. To change a sensor's Modbus address, use the sensor setup screen in the app (only connect one sensor at a time).

## Building

Dependencies (Debian/Raspbian):
```bash
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libmodbus-dev
```

Build:
```bash
mkdir build && cd build
cmake ..
make -j4
```

## USB-RS485 adapter recovery

The FTDI USB-RS485 adapter can get stuck after restarts, showing `Invalid CRC` or `Connection timed out` errors. The app will automatically attempt a USB reset after 10 consecutive failures.

Manual software reset:
```bash
echo 0 | sudo tee /sys/bus/usb/devices/1-1.3/authorized
sleep 1
echo 1 | sudo tee /sys/bus/usb/devices/1-1.3/authorized
sudo systemctl restart heightmatic
```

If that doesn't work, physically unplug and replug the adapter.

## Systemd service

The app runs as a systemd service on the Pi:

```ini
[Unit]
Description=Height-o-meter
After=multi-user.target

[Service]
Type=simple
User=rpi
Environment=SDL_VIDEODRIVER=kmsdrm
WorkingDirectory=/home/rpi/height-o-meter
ExecStart=/home/rpi/height-o-meter/build/heightmatic
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

Install: `sudo cp heightmatic.service /etc/systemd/system/ && sudo systemctl enable heightmatic`

## Display rotation

The Pi uses `display_lcd_rotate=1` in `/boot/firmware/config.txt` for portrait orientation. The SDL kmsdrm driver does not respect this - the app renders in landscape 800x480.
