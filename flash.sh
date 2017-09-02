# Set baudrate to 115200 before sending the reset command
#stty -F /dev/ttyUSB0 115200
#echo R > /dev/ttyUSB0
#sleep 1s
#stty -F /dev/ttyUSB0 57600
avrdude -c arduino -b 57600 -P /dev/ttyUSB0 -p atmega328p -vv -U flash:w:/tmp/arduino_build_198198/throttle_controller.ino.hex 

