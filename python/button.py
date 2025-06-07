import os
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, CheckButtons
from matplotlib.backend_bases import MouseEvent

class ButtonProcessor:
    axes: plt.Axes
    button: Button
    
    def __init__(self, axes: plt.Axes, label: str, callback, parameter):
        self.axes = axes
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
    axes: plt.Axes
    button: Button
    
    def __init__(self, axes: plt.Axes, label: str, callback = None, callback_parameter = None, state: bool = False):
        self.axes = axes
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

class ButtonCycle:
    index: int
    labels: list[str]
    values: list
    axes: plt.Axes
    button: Button
    
    def __init__(self, axes: plt.Axes, labels: list[str], values: list[str] = None, callback = None):
        self.index = 0
        self.labels = labels
        self.values = labels if values == None else values
        self.axes = axes
        self.button = Button(axes, labels[0])
        self.button.on_clicked(self.process)
        self.callback = callback
    
    def process(self, event):
        self.index = (self.index + 1) % len(self.labels)
        self.button.label.set_text(self.labels[self.index])
        if (self.callback != None):
            self.callback()
    
    def delete(self):
        self.button = None
        self.axes = None
        
    def get_value(self) -> str:
        return self.values[self.index]
    def get_label(self) -> str:
        return self.labels[self.index]

    def set_visible(self, state: bool):
        self.axes.set_visible(state)
    
class ButtonCheck:
    axes: plt.Axes
    button: CheckButtons
    
    def __init__(self, axes: plt.Axes, labels: list[str], callback = None):
        self.axes = axes
        self.button = CheckButtons(axes, labels)
        self.button.on_clicked(self.process)
        self.callback = callback
    
    def process(self, event):
        if (self.callback != None):
            self.callback()
    
    def delete(self):
        self.button = None
        self.axes = None

    def set_visible(self, state: bool):
        self.axes.set_visible(state)
    