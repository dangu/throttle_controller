import Tkinter as tk
import serial
import flash
import tkFileDialog
import time
import os
from compiler.syntax import check

class Settings:
    pass

settings = Settings()
settings.rpmMax     = 2500
settings.rpmMin     = 0
settings.rpmStart   = 800
settings.servoMax   = 100
settings.servoMin   = 0
settings.scalesLength = 300

# Serial commands understood by the control unit
CMD_ENABLE_EXT_N_ENG        = 'a'
CMD_DISABLE_EXT_N_ENG       = 'b'
CMD_ENABLE_EXT_SERVO_POS    = 'c'
CMD_DISABLE_EXT_SERVO_POS   = 'd'
CMD_SET_PID_SERVO           = 'e'
CMD_SET_PID_N_ENG           = 'f'
CMD_DISP_PID_PARAMS         = 'g'
CMD_DISP_VALUES             = 'i'

RESP_DISP_PID_PARAMS        = 'G'
RESP_DISP_VALUES            = 'I'

class Sercom(serial.Serial):
    def __init__(self, port, baudrate):
        """Init"""
        serial.Serial.__init__(self)
        self.port = port
        self.baudrate = baudrate
        self.timeout = 1
        self.msgOutQueue = []
        
    def writeQueued(self,cmd):
        """Add a message to the out queue"""
        self.msgOutQueue.append(cmd)
        
    def isOutQueueEmpty(self):
        """Return true if the out queue is empty"""
        return len(self.msgOutQueue)==0
    
    def processOutQueue(self):
        """Process the out queue to send messages one by one"""
        if len(self.msgOutQueue)>0:
            print "Processing one message"
            self.write(self.msgOutQueue[0])
            self.msgOutQueue = self.msgOutQueue[1:]  # Remove the first element

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
        
        frameScales = tk.Frame(parent, bd=2, relief=tk.GROOVE)
        frameScales.grid(row=2,column=0)
        
        frameServo = tk.Frame(parent, bd=2, relief=tk.GROOVE)
        frameServo.grid(row=2,column=1)

        frameAD = tk.Frame(parent, bd=2, relief=tk.GROOVE)
        frameAD.grid(row=2,column=2)

        frameConvert = tk.Frame(parent, bd=2, relief=tk.GROOVE)
        frameConvert.grid(row=1,column=2, columnspan=2, sticky="nsew")
                        
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

        labelUMin = tk.Label(framePID, text="uMin")
        labelUMin.grid(row=2, column=4, sticky="nsew")

        labelUMax = tk.Label(framePID, text="uMax")
        labelUMax.grid(row=2, column=5, sticky="nsew")
        
        # PID values
        spinboxWidth = 5
        self.spinPidServoP = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidServoP.grid(row=3, column=1)
        self.spinPidServoP.delete(0,"end")
        self.spinPidServoP.insert(tk.END,"0")

        self.spinPidServoI = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidServoI.grid(row=3, column=2)
        self.spinPidServoI.delete(0,"end")
        self.spinPidServoI.insert(tk.END,"0")

        self.spinPidServoD = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidServoD.grid(row=3, column=3)
        self.spinPidServoD.delete(0,"end")
        self.spinPidServoD.insert(tk.END,"0")
         
        self.spinPidServoUMin = tk.Spinbox(framePID, width=spinboxWidth, from_=-1000, to=1000, increment=0.1)
        self.spinPidServoUMin.grid(row=3, column=4)
        self.spinPidServoUMin.delete(0,"end")
        self.spinPidServoUMin.insert(tk.END,"0")
        
        self.spinPidServoUMax = tk.Spinbox(framePID, width=spinboxWidth, from_=-1000, to=1000, increment=0.1)
        self.spinPidServoUMax.grid(row=3, column=5)
        self.spinPidServoUMax.delete(0,"end")
        self.spinPidServoUMax.insert(tk.END,"0")
        
        btnSetPidServo = tk.Button(framePID, text="Get", command=self.cbGetPidParams)
        btnSetPidServo.grid(row=3, column=6, sticky="nsew", padx=2, pady=2, rowspan = 2)

        btnSetPidServo = tk.Button(framePID, text="Set", command=self.cbSetPidServo)
        btnSetPidServo.grid(row=3, column=7, sticky="nsew", padx=2, pady=2)

        self.spinPidNEngP = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidNEngP.grid(row=4, column=1)
        self.spinPidNEngP.delete(0,"end")
        self.spinPidNEngP.insert(tk.END,"0")
        
        self.spinPidNEngI = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidNEngI.grid(row=4, column=2)
        self.spinPidNEngI.delete(0,"end")
        self.spinPidNEngI.insert(tk.END,"0")
        
        self.spinPidNEngD = tk.Spinbox(framePID, width=spinboxWidth, from_=0, to=10, increment=0.01)
        self.spinPidNEngD.grid(row=4, column=3)
        self.spinPidNEngD.delete(0,"end")
        self.spinPidNEngD.insert(tk.END,"0")
        
        self.spinPidNEngUMin = tk.Spinbox(framePID, width=spinboxWidth, from_=-1000, to=1000, increment=0.1)
        self.spinPidNEngUMin.grid(row=4, column=4)
        self.spinPidNEngUMin.delete(0,"end")
        self.spinPidNEngUMin.insert(tk.END,"0")
        
        self.spinPidNEngUMax = tk.Spinbox(framePID, width=spinboxWidth, from_=-1000, to=1000, increment=0.1)
        self.spinPidNEngUMax.grid(row=4, column=5)
        self.spinPidNEngUMax.delete(0,"end")
        self.spinPidNEngUMax.insert(tk.END,"0")

        btnSetPidNEng = tk.Button(framePID, text="Set", command=self.cbSetPidNEng)
        btnSetPidNEng.grid(row=4, column=7, sticky="nsew", padx=2, pady=2)

 
        # RPM data
        labelScaleRpmTarget = tk.Label(frameScales, text="Target RPM")
        labelScaleRpmTarget.grid(row=0, column=0, sticky="nsew")
        
        checkboxRpmTarget = tk.Checkbutton(frameScales, text="Override")
        checkboxRpmTarget.grid(row=1, column=0, sticky="nsew")

        btnUpdateData = tk.Button(frameScales, text="Display Data", command=self.cbDisplayData)
        btnUpdateData.grid(row=1, column=1, sticky="nsew", padx=2, pady=2)

        labelScaleRpmMeasured = tk.Label(frameScales, text="Measured RPM")
        labelScaleRpmMeasured.grid(row=0, column=1, sticky="nsew")
             
        self.scaleRpmTarget = tk.Scale(frameScales,  from_=settings.rpmMax, to=settings.rpmMin, length=settings.scalesLength, tickinterval=100, command = self.cbScale1)
        self.scaleRpmTarget.set(settings.rpmStart)
        self.scaleRpmTarget.grid(row=2, column=0)
         
        self.scaleRpmMeasured = tk.Scale(frameScales,  from_=settings.rpmMax, to=settings.rpmMin, length=settings.scalesLength, command = self.cbScale1)
        self.scaleRpmMeasured.grid(row=2, column=1)
 
        # A/D converter
        labelScaleServoPosMeasured = tk.Label(frameAD, text="Servo measured")
        labelScaleServoPosMeasured.grid(row=0, column=1, sticky="nsew")
        labelScalePotMeasured = tk.Label(frameAD, text="Pot measured")
        labelScalePotMeasured.grid(row=0, column=2, sticky="nsew")

        labelScaleServoPosTarget = tk.Label(frameAD, text="Servo target")
        labelScaleServoPosTarget.grid(row=0, column=3, sticky="nsew")
        labelScaleServoPosVirtual = tk.Label(frameAD, text="Servo virtual")
        labelScaleServoPosVirtual.grid(row=0, column=4, sticky="nsew")
        labelScalePotVirtual = tk.Label(frameAD, text="Pot virtual")
        labelScalePotVirtual.grid(row=0, column=5, sticky="nsew")
            
        labelMin = tk.Label(frameAD, text="Min")
        labelMin.grid(row=1, column=0, sticky="nsew")
        labelMax = tk.Label(frameAD, text="Max")
        labelMax.grid(row=2, column=0, sticky="nsew")
                
        self.spinServoMinAD = tk.Spinbox(frameAD, width=spinboxWidth, from_=0, to=1024, increment=1)
        self.spinServoMinAD.grid(row=1, column=1, sticky="e")
        self.spinServoMinAD.delete(0,"end")
        self.spinServoMinAD.insert(tk.END,"0")

        self.spinServoMaxAD = tk.Spinbox(frameAD, width=spinboxWidth, from_=0, to=1024, increment=1)
        self.spinServoMaxAD.grid(row=2, column=1, sticky="e")
        self.spinServoMaxAD.delete(0,"end")
        self.spinServoMaxAD.insert(tk.END,"0")
        
        self.spinPotMinAD = tk.Spinbox(frameAD, width=spinboxWidth, from_=0, to=1024, increment=1)
        self.spinPotMinAD.grid(row=1, column=2, sticky="e")
        self.spinPotMinAD.delete(0,"end")
        self.spinPotMinAD.insert(tk.END,"0")

        self.spinPotMaxAD = tk.Spinbox(frameAD, width=spinboxWidth, from_=0, to=1024, increment=1)
        self.spinPotMaxAD.grid(row=2, column=2, sticky="e")
        self.spinPotMaxAD.delete(0,"end")
        self.spinPotMaxAD.insert(tk.END,"0")
          
        self.scaleServoPosMeasured = tk.Scale(frameAD,  from_=1024, to=0, length=settings.scalesLength, tickinterval=256, command = self.cbScale1)
        self.scaleServoPosMeasured.grid(row=3, column=1)
      
        self.scalePotMeasured = tk.Scale(frameAD,  from_=1024, to=0, length=settings.scalesLength, command = self.cbScale1)
        self.scalePotMeasured.grid(row=3, column=2)

        checkboxServoTarget = tk.Checkbutton(frameAD, text="Override")
        checkboxServoTarget.grid(row=2, column=3, sticky="nsew")
        
        self.scaleServoPosTarget = tk.Scale(frameAD,  from_=100, to=0, length=settings.scalesLength, tickinterval=20, command = self.cbScale1)
        self.scaleServoPosTarget.grid(row=3, column=3)
 
        self.scaleServoPosVirtual = tk.Scale(frameAD,  from_=100, to=0, length=settings.scalesLength, command = self.cbScale1)
        self.scaleServoPosVirtual.grid(row=3, column=4)
      
        self.scalePotVirtual = tk.Scale(frameAD,  from_=100, to=0, length=settings.scalesLength, command = self.cbScale1)
        self.scalePotVirtual.grid(row=3, column=5)
                
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
                print "Waiting..."
                line = self.serialPort.readline()
                print "Rx: {}".format(line.strip())
                self.handleResponse(line)
            
            self.after(10, self.cbReadSerialPort)
            
    def handleResponse(self, response):
        """Handle the response from the control unit"""
        if len(response)>0:
            respList = response.split()
            if len(respList)>0:
                cmd = respList[0]
                if cmd == RESP_DISP_PID_PARAMS:
                    # Get all PID parameters
                    if len(respList) == (1+5+5):
                        i=1
                        for spinbox in [self.spinPidServoP,
                                        self.spinPidServoI,
                                        self.spinPidServoD,
                                        self.spinPidServoUMin,
                                        self.spinPidServoUMax,
                                        self.spinPidNEngP,
                                        self.spinPidNEngI,
                                        self.spinPidNEngD,
                                        self.spinPidNEngUMin,
                                        self.spinPidNEngUMax,
                                        ]:
                            spinbox.delete(0,"end")
                            spinbox.insert(tk.END,"{:g}".format(float(respList[i])))
                            i+=1
                elif cmd == RESP_DISP_VALUES:
                    if len(respList) == (1+3):
                        print "Data: {}".format(respList[1:])
                        self.scaleRpmMeasured.set(float(respList[1]))
                        self.scaleServoPosMeasured.set(float(respList[2]))
                        self.scalePotMeasured.set(float(respList[3]))
        
    def cbScale1(self, var2):
        """Callback for scale 1"""
        print "Value {}".format(var2)
        
    def cbReset(self):
        """Send a reset command"""
        if self.serialPort.isOpen():
            self.serialPort.writeQueued("R\n")

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
        """Set the parameters for the servo PID"""
        if(self.serialPort.isOpen()):
            print "Set servo PID parameters"
            cmd = "{} {} {} {} {} {}\n".format(CMD_SET_PID_SERVO,
                                       self.spinPidServoP.get(),
                                       self.spinPidServoI.get(),
                                       self.spinPidServoD.get(),
                                       self.spinPidServoUMin.get(),
                                       self.spinPidServoUMax.get())
            self.serialPort.writeQueued(cmd)
  
    def cbSetPidNEng(self):
        """Set the parameters for the engine speed PID"""
        if(self.serialPort.isOpen()):
            print "Set engine speed PID parameters"
            cmd = "{} {} {} {} {} {}\n".format(CMD_SET_PID_N_ENG,
                                       self.spinPidNEngP.get(),
                                       self.spinPidNEngI.get(),
                                       self.spinPidNEngD.get(),
                                       self.spinPidNEngUMin.get(),
                                       self.spinPidNEngUMax.get())
            self.serialPort.writeQueued(cmd)
            
    def cbGetPidParams(self):
        """Get the parameters for the PID controllers"""
        if self.serialPort.isOpen():
            print "Get PID parameters"
            cmd = "{}\n".format(CMD_DISP_PID_PARAMS)
            self.serialPort.writeQueued(cmd)
            
    
    def cbDisplayData(self):
        """Update the display data"""
        if self.serialPort.isOpen():
            if(self.serialPort.isOutQueueEmpty()):
                print "Update display data"
                cmd = "{}\n".format(CMD_DISP_VALUES)
                self.serialPort.writeQueued(cmd)            
                    
            self.serialPort.processOutQueue()
            self.after(100, self.cbDisplayData)

def run():
    """Run graphics"""
    root = tk.Tk()
    app = Gui(root, "Throttle controller", Sercom("/dev/ttyUSB0", 19200))
    #app.read_serial()
    app.mainloop()

if __name__=="__main__":
    run()
    

