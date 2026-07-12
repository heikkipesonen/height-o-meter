while true; do
  mbpoll -m rtu -b 9600 -P none -a 80 -t 4 -r 61 -c 3 /dev/ttyUSB0
  sleep 0.5
done
