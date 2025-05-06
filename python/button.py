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

class ButtonToggle:
    state: bool
    
    def __init__(self, axes: plt.Axes, label: str, callback = None, callback_parameter = None, state: bool = False):
        self.button = Button(axes, label)
        self.button.on_clicked(self.process)
        self.callback = callback
        self.callback_parameter = callback_parameter
        self.state = state
    
    def process(self, event):
        self.state = not(self.state)
        if (self.callback != None):
            if (self.callback_parameter != None):
                self.callback(self.callback_parameter)
            else:
                self.callback()
    
    def delete(self):
        self.button = None