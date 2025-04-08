import os
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent

class ButtonProcessor:
    def __init__(self, axes: plt.Axes, label: str, callback, parameter):
        self.button = Button(axes, label)
        self.button.on_clicked(self.process)
        self.callback = callback
        self.parameter = parameter
    
    def process(self, event):
        self.callback(self.parameter)
    
    def delete(self):
        self.button = None