import Tkinter as tk

def run():
    """Run graphics"""
    top = Tkinter.Tk()
    

    filename = r"../game/src/test.chr"
    
    # Load a group of tiles from file
    tileGroup.loadFromFile(filename) 
    
    # Set the colors to use with the tiles
    palette = Palette()
    palette.setColors(['black', 'green', 'yellow', 'grey'])
    
    nTilesX = 16
    nTilesY = 32
    xScale  = 3
    yScale = xScale
    C = Tkinter.Canvas(top, bg="grey", width=8*nTilesX*xScale, height=8*nTilesY*yScale, cursor = "crosshair")
    p = CanvasPlotter(C)
    p.setScale(xScale, yScale)
    
xOffset=0