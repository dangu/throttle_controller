echo R > /dev/ttyUSB0
#sleep 1s
#avrdude -c arduino -b 57600 -P /dev/ttyUSB0 -p atmega328p -vv -U flash:w:/tmp/arduino_build_556325/throttle_controller.ino.hex 
