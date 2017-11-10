import Tkinter as tk
import serial
import flash
import tkFileDialog
import time
import os

class Settings:
    pass

settings = Settings()
settings.rpmMax     = 2500
settings.rpmMin     = 400
settings.rpmStart   = 800
settings.servoMax   = 100
settings.servoMin   = 0

class Sercom(serial.Serial):
    def __init__(self, port, baudrate):
        """Init"""
        serial.Serial.__init__(self)
        self.port = port
        self.baudrate = baudrate

class Gui(tk.Frame):
    def __init__(self, parent, title, serialPort):
        """Init"""
        tk.Frame.__init__(self, parent)
        self.serialPort = serialPort
        self.initialHexFileDir = r"/tmp"
        
        self.btnSerial=tk.Button(parent, text="Open serial port", command=self.cbOpenCloseSerialPort)
        self.btnSerial.grid(row=0,column=0)

        self.btnReadSerial=tk.Button(parent, text="Read serial port", command=self.cbReadSerialPort)
        self.btnReadSerial.grid(row=0,column=1)
        
        btnReset = tk.Button(parent, text="Reset", command=self.cbReset)
        btnReset.grid(row=0, column=2, sticky="nsew", padx=2, pady=2)

        btnFlash = tk.Button(parent, text="Flash", command=self.cbFlash)
        btnFlash.grid(row=0, column=3, sticky="nsew", padx=2, pady=2)
        
        framePID = tk.Frame(parent, bd=2, relief=tk.GROOVE)
        framePID.grid(row=1)
        
        labelServo = tk.Label(framePID, text="Servo")
        labelServo.grid(row=3, column=0, sticky="e")

        labelEng = tk.Label(framePID, text="nEng")
        labelEng.grid(row=4, column=0, sticky="e")

        labelP = tk.Label(framePID, text="P")
        labelP.grid(row=2, column=1, sticky="nsew")

        labelI = tk.Label(framePID, text="I")
        labelI.grid(row=2, column=2, sticky="nsew")

        labelD = tk.Label(framePID, text="D")
        labelD.grid(row=2, column=3, sticky="nsew")
        
        # PID values
        spinboxWidth = 5
        spinPidServoP = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidServoP.grid(row=3, column=1)
        spinPidServoP.delete(0,"end")
        spinPidServoP.insert(tk.END,"0")
 
        spinPidServoI = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidServoI.grid(row=3, column=2)
        spinPidServoI.delete(0,"end")
        spinPidServoI.insert(tk.END,"0")
  
        spinPidServoD = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidServoD.grid(row=3, column=3)
        spinPidServoD.delete(0,"end")
        spinPidServoD.insert(tk.END,"0")
         
        btnSetPidServo = tk.Button(framePID, text="Set", command=self.cbSetPidServo)
        btnSetPidServo.grid(row=3, column=4, sticky="nsew", padx=2, pady=2)

        spinPidNEngP = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidNEngP.grid(row=4, column=1)
        spinPidNEngP.delete(0,"end")
        spinPidNEngP.insert(tk.END,"0")
          
        spinPidNEngI = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidNEngI.grid(row=4, column=2)
        spinPidNEngI.delete(0,"end")
        spinPidNEngI.insert(tk.END,"0")
        
        spinPidNEngD = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        spinPidNEngD.grid(row=4, column=3)
        spinPidNEngD.delete(0,"end")
        spinPidNEngD.insert(tk.END,"0")

        btnSetPidNEng = tk.Button(framePID, text="Set", command=self.cbSetPidNEng)
        btnSetPidNEng.grid(row=4, column=4, sticky="nsew", padx=2, pady=2)

# 
# 
#         self.scaleRpmTarget = tk.Scale(parent,  from_=settings.rpmMax, to=settings.rpmMin, length=500, tickinterval=100, command = self.cbScale1)
#         self.scaleRpmTarget.set(settings.rpmStart)
#         self.scaleRpmTarget.grid(row=2, column=0)
#         
#         self.scaleRpmMeasured = tk.Scale(parent,  from_=settings.rpmMax, to=settings.rpmMin, length=500, command = self.cbScale1)
#         self.scaleRpmMeasured.grid(row=2, column=1)
# 
#         self.scaleServoPosMeasured = tk.Scale(parent,  from_=settings.servoMax, to=settings.servoMin, length=500, command = self.cbScale1)
#         self.scaleServoPosMeasured.grid(row=2, column=2)
#         
         
    def cbOpenCloseSerialPort(self):
        """Open or close serial port"""
        if self.serialPort.isOpen():
            self.serialPort.close()
            self.btnSerial['text'] = "Open serial port"
        else:
            self.serialPort.open()
            self.btnSerial['text'] = "Close serial port"

    def cbReadSerialPort(self):
        """Read serial port"""
        if self.serialPort.isOpen():
            if self.serialPort.inWaiting() != 0:
                line = self.serialPort.readline()
                print line.strip()
            
            self.after(10, self.cbReadSerialPort)
        
    def cbScale1(self, var2):
        """Callback for scale 1"""
        print "Value {}".format(var2)
        
    def cbReset(self):
        """Send a reset command"""
        if self.serialPort.isOpen():
            self.serialPort.write("R\n")

    def cbFlash(self):
        """Flash with new software"""
        if self.serialPort.isOpen():
            filename = tkFileDialog.askopenfilename(initialdir = self.initialHexFileDir
                                                    ,title = "Select hex file",
                                                    filetypes =(("hex files", "*.hex")
                                                               ,("all files","*.*")))

            if filename:
                self.cbReset()
                # self.serialPort.close() # Need to close serial port before flashing
                flash.Flasher().flash(filename)
                print self.serialPort.baudrate
                
                # Save the directory for opening the next time
                self.initialHexFileDir = os.path.dirname(filename)
                
    def cbSetPidServo(self):
        """Set the values for the servo PID"""
        print "Set servo PID values"
  
    def cbSetPidNEng(self):
        """Set the values for the engine speed PID"""
        print "Set engine speed PID values"
           
    def read_serial(self):
        """
        Check for input from the serial port. On fetching a line, parse
        the sensor values and append to the stored data and post a replot
        request.
        """
        if self.serialPort.inWaiting() != 0:
            line = self.serialPort.readline()
            line = line.decode('ascii').strip("\r\n")
            if line[0:3] != "MAG":
                print(line) # line not a valid sensor result.
            else:
                try:
                    data = line.split("\t")
                    x, y, z = data[1], data[2], data[3]
                    self.append_values(x, y, z)
                    self.after_idle(self.replot)
                except Exception as e:
                    print(e)
        self.after(10, self.read_serial)

def run():
    """Run graphics"""
    root = tk.Tk()
    app = Gui(root, "Throttle controller", Sercom("/dev/ttyUSB0", 19200))
    #app.read_serial()
    app.mainloop()

if __name__=="__main__":
    run()
    

