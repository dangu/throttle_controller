import Tkinter as tk
import serial
import flash
import tkFileDialog
import time

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
        
        self.btnSerial=tk.Button(parent, text="Open serial port", command=self.cbOpenCloseSerialPort)
        self.btnSerial.grid(row=0,column=0)

        self.btnReadSerial=tk.Button(parent, text="Read serial port", command=self.cbReadSerialPort)
        self.btnReadSerial.grid(row=0,column=1)
        
        btnReset = tk.Button(parent, text="Reset", command=self.cbReset)
        btnReset.grid(row=0, column=2, sticky="nsew", padx=2, pady=2)

        btnFlash = tk.Button(parent, text="Flash", command=self.cbFlash)
        btnFlash.grid(row=0, column=3, sticky="nsew", padx=2, pady=2)

        self.scaleRpmTarget = tk.Scale(parent,  from_=settings.rpmMax, to=settings.rpmMin, length=500, tickinterval=100, command = self.cbScale1)
        self.scaleRpmTarget.set(settings.rpmStart)
        self.scaleRpmTarget.grid(row=1, column=0)
        
        self.scaleRpmMeasured = tk.Scale(parent,  from_=settings.rpmMax, to=settings.rpmMin, length=500, command = self.cbScale1)
        self.scaleRpmMeasured.grid(row=1, column=1)

        self.scaleServoPosMeasured = tk.Scale(parent,  from_=settings.servoMax, to=settings.servoMin, length=500, command = self.cbScale1)
        self.scaleServoPosMeasured.grid(row=1, column=2)
        
         
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
            self.serialPort.write("R")

    def cbFlash(self):
        """Flash with new software"""
        if self.serialPort.isOpen():
            filename = tkFileDialog.askopenfilename(initialdir = "/tmp",title = "Select hex file",filetypes =(("hex files", "*.hex")
                                                                                                    ,("all files","*.*")))

            if filename:
                self.cbReset()
               # self.serialPort.close() # Need to close serial port before flashing
                flash.Flasher().flash(filename)
                print self.serialPort.baudrate
                
           
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

class Debug:
    def __init__(self, parent):
    # create a Frame for the Text and Scrollbar
        txt_frm = tk.Frame(parent, width=600, height=600)
        txt_frm.pack(fill="both", expand=True)
        # ensure a consistent GUI size
        txt_frm.grid_propagate(False)
        # implement stretchability
        txt_frm.grid_rowconfigure(0, weight=1)
        txt_frm.grid_columnconfigure(0, weight=1)
        
        btn1 = tk.Button(txt_frm, text="Read config", command=self.readConfig)
        btn1.grid(row=1, column=0, sticky="nsew", padx=2, pady=2)

    # create a Text widget
        self.textbox1 = tk.Text(txt_frm, borderwidth=3, relief="sunken")
        #self.txt.config(font=("consolas", 12), undo=True, wrap='word')
        self.textbox1.grid(row=0, column=0, sticky="nsew", padx=2, pady=2)

    # create a Scrollbar and associate it with txt
        scrollb = tk.Scrollbar(txt_frm, command=self.textbox1.yview)
        scrollb.grid(row=0, column=1, sticky='nsew')
        self.textbox1['yscrollcommand'] = scrollb.set
        
    def __init__2(self):
        top = tk.Tk()
        
        filename = r"../game/src/test.chr"
        
        C = tk.Canvas(top, width=150, height=150)
        C.pack()
        btn1 = tk.Button(top, text="Read config", command=self.readConfig)
        btn1.pack()
        self.textbox1 = tk.Text(top)
        self.textbox1.pack()

        self.textbox1.insert(tk.INSERT, "click here!")
        
        # create a Scrollbar and associate it with txt
        scrollb = tk.Scrollbar(top, command=self.textbox1.yview)
        self.textbox1['yscrollcommand'] = scrollb.set

    def enter(self):
        """Enter callback"""
        print "Enter"
        
    def leave(self):
        """Leave callback"""
        print leave
        
    def click(self):
        """Callback for click"""
        print "Click!"
        
    def readConfig(self):
        """Read configuration"""
        print "Read config..."
        #self.textbox1.insert(0,"Read config...")
        self.textbox1.insert(tk.END, "click here!\n1\n")
       
    def initSerial(self):
        """Init serial communication"""

        self.ser.open()


def run():
    """Run graphics"""
    root = tk.Tk()
    app = Gui(root, "Throttle controller", Sercom("/dev/ttyUSB0", 19200))
    #app.read_serial()
    app.mainloop()

if __name__=="__main__":
    run()
    

