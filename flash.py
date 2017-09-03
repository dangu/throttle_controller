import serial
import subprocess
import time
        
class Flasher:
    def __init__(self):
        """Init"""
        pass
    def sendReset(self):
        """Send the reset command to the Arduino"""
        ser = serial.Serial('/dev/ttyUSB0')
        ser.baudrate = 57600
        
        ser.write("R")
        ser.close()
      #  ser = serial.Serial('/dev/ttyUSB0')
       # ser.baudrate = 115200

       # time.sleep(1)
      #  ser.write([0x30, 0x20])

      #  print ser.read_all()
      #  ser.close()
     
    def flash(self):
        buildDir = "/tmp/arduino_build_188983"

        avrdudeCmd = ['avrdude', '-c', 'arduino', '-b', '57600',
                     '-P', '/dev/ttyUSB0', '-p',
                     'atmega328p', '-vv', '-U',
                     'flash:w:%s/throttle_controller.ino.hex:i' % buildDir]

        avrdudeCmd = ['./flash.sh']
        print " ".join(avrdudeCmd)
        proc = subprocess.Popen(avrdudeCmd,
                             #   stdout=file_out, stderr=subprocess.PIPE)
                                stdout=subprocess.PIPE, shell=True)
        log_string = ""
      # output log of what happened (log_string is the var that is bound to the status label at the bottom of the GUI)
        proc.wait()
        avrdude_err = proc.communicate()[1]
        
        if (avrdude_err == ''):
          log_string = "\nFlash successful"
        else:
          log_string = "\nLog:\n%s\n" % avrdude_err
          
        print log_string
    
        # for line in proc.stderr:
         #   sys.stdout.write(line)
            # log_file.write(line)
        #proc.wait()

    def flash_hex(self, make_tempfile):
      # chain together the avr dude command with flags
      avrdude_cmd = 'avrdude -qq -P usb -c %s -p attiny45 -b 15 -e -U flash:w:%s:i -U eeprom:w:%s:i' % (123, " ", make_tempfile)
      # print avrdude_cmd
      # open subprocess to run avrdude
      avrdude = subprocess.Popen(avrdude_cmd, 
                                 stderr=subprocess.PIPE, shell=True)
      avrdude_err = avrdude.communicate()[1]
    
      log_string = ""
      # output log of what happened (log_string is the var that is bound to the status label at the bottom of the GUI)
      if (avrdude_err == ''):
        log_string = '\nAll done! Your radio was successfully flashed with frequency %s.\n' % e1.get()
      else:
        log_string = '\nLog:\n%s\n' % avrdude_err
        
      print log_string
    
if __name__ == "__main__":
   # for i in range(0,20,2):
    sleeptime=0
    print "Sending reset"
    flasher = Flasher()
    flasher.sendReset()
    print "Should now reset"
    print "Sleepint %.1f and then flashing" %sleeptime
    time.sleep(sleeptime)
    flasher.flash()
    print "Ready?"

