import Tkinter as tk
import serial

class Sercom(serial.Serial):
    def __init__(self, port):
        """Init"""
        serial.Serial.__init__(self)
        self.port = port



def run():
    """Run graphics"""
    top = tk.Tk()
    

    filename = r"../game/src/test.chr"
    
    C = tk.Canvas(top, width=150, height=150)
    C.pack()
    
    tk.mainloop()

if __name__=="__main__":
    ser = Sercom("/dev/ttyUSB0")
    ser.baudrate = 19200
    ser.open()
    while(True):
        print ser.readline()
#    run()