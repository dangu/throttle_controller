import Tkinter as tk
import serial
from wx.lib.agw.balloontip import BT_LEAVE

class Sercom(serial.Serial):
    def __init__(self, port):
        """Init"""
        serial.Serial.__init__(self)
        self.port = port

class Gui:
    def __init__(self):
        self.root = tk.Tk()

    # create a Frame for the Text and Scrollbar
        txt_frm = tk.Frame(self.root, width=600, height=600)
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
        self.ser = Sercom("/dev/ttyUSB0")
        self.ser.baudrate = 19200
        self.ser.open()


def run():
    """Run graphics"""
    gui=Gui()
    gui.initSerial()
    tk.mainloop()

if __name__=="__main__":
    run()
    

