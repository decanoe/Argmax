import matplotlib.pyplot as plt
from matplotlib.transforms import Bbox
from matplotlib.widgets import Button, CheckButtons, TextBox
from matplotlib.backend_bases import MouseButton

def clamp(n, smallest, largest): return max(smallest, min(n, largest))

class KeyHoldEvent:
    holded_keys: list[str]
    
    def __init__(self, figure: plt.Figure):
        self.holded_keys = []
        figure.canvas.mpl_connect('key_press_event', lambda evt: self.add_key(evt.key))
        figure.canvas.mpl_connect('key_release_event', lambda evt: self.remove_key(evt.key))
    
    def add_key(self, key: str):
        if (key not in self.holded_keys):
            self.holded_keys.append(key)
    def remove_key(self, key: str):
        if (key in self.holded_keys):
            self.holded_keys.remove(key)
    
    def is_key_held(self, key: str) -> bool:
        return key in self.holded_keys

class ButtonProcessor:
    axes: plt.Axes
    button: Button
    
    def __init__(self, axes: plt.Axes, label: str, callback, parameter):
        self.axes = axes
        self.axes._is_button = True # custom tag
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
        self.axes._is_button = True # custom tag
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
        self.axes._is_button = True # custom tag
        self.button = Button(axes, labels[0])
        self.button.on_clicked(self.process)
        self.callback = callback
    
    def process(self, event):
        if event.button is MouseButton.LEFT:
            self.index = (self.index + 1) % len(self.labels)
        elif event.button is MouseButton.RIGHT:
            self.index = (self.index + len(self.labels) - 1) % len(self.labels)
        self.button.label.set_text(self.labels[self.index])
        if (self.callback != None):
            self.callback()
    
    def delete(self):
        self.button = None
        self.axes = None
        
    def get_value(self) -> str:
        return self.values[self.index]
    def set_values(self, values: list[str]):
        self.values = values
    def get_label(self) -> str:
        return self.labels[self.index]
    def set_labels(self, labels: list[str]):
        self.labels = labels

    def set_visible(self, state: bool):
        self.axes.set_visible(state)

    def set_to(self, index: int):
        self.index = index % len(self.labels)
        self.button.label.set_text(self.labels[self.index])

class ButtonCheck:
    axes: plt.Axes
    button: CheckButtons
    position: Bbox
    labels: list[str]
    values: list[str]
    states: list[bool]
    key_hold: KeyHoldEvent
    last_clicked_label: str
    
    def __init__(self, axes: plt.Axes, labels: list[str], values: list[str] = None, callback = None):
        self.axes = axes
        self.axes._is_button = True # custom tag
        self.button = CheckButtons(axes, labels)
        self.button.on_clicked(self.process)
        self.callback = callback
        self.key_hold = None
        self.last_clicked_label = None
        self.labels = labels
        self.values = labels if values == None else values
        self.states = [False] * len(self.values)
        
        self.position = axes.get_position()
    
    def add_key_hold_shortcuts(self, key_hold: KeyHoldEvent):
        self.key_hold = key_hold
    def _process_maj_check(self, label1: str, label2: str):
        index1: int = self.labels.index(label1)
        index2: int = self.labels.index(label2)
        self.button.eventson = False
        for i in range(min(index1, index2), max(index1, index2) + 1):
            self.check_index(i, self.states[index2])
        self.button.eventson = True
    def process(self, clicked_label):
        index = self.labels.index(clicked_label)
        self.states[index] = clicked_label in self.button.get_checked_labels()
        
        if (self.key_hold != None):
            if (clicked_label != None and self.last_clicked_label != None and self.key_hold.is_key_held("shift")):
                self._process_maj_check(self.last_clicked_label, clicked_label)
        self.last_clicked_label = clicked_label
        
        if (self.callback != None):
            self.callback()
    
    def delete(self):
        self.button = None
        self.axes = None

    def set_visible(self, state: bool):
        self.axes.set_visible(state)
        
        if (state):
            self.axes.set_position(self.position)
        else:
            self.axes.set_position([100, 100, .1, .1])
    
    def check_index(self, index: int, state: bool = True):
        self.button.set_active(index, state)
        self.states[index] = state
    def check(self, key: str, state: bool = True):
        self.check_index(self.values.index(key), state)
    def check_all(self, keys: list[str], state: bool = True):
        for key in keys:
            self.check(key, state)
    
    def is_checked(self, key: str) -> bool:
        if (key not in self.values):
            return False
        return self.states[self.values.index(key)]
class ButtonCheckPartial(ButtonCheck):
    displayed: list[bool]
    
    def __init__(self, axes: plt.Axes, labels: list[str], values: list[str] = None, callback = None):
        super().__init__(axes, labels, values, callback)
        self.displayed = [True] * len(self.values)

    def filter(self, keywords: list[str]):
        self.last_clicked_label = None

        self.displayed = [False] * len(self.values)
        for i in range(len(self.displayed)):
            found: str = True
            for keyword in keywords:
                if not keyword.startswith('!') and keyword not in self.labels[i]:
                    found = False
                    break
                if keyword.startswith('!') and keyword.removeprefix('!') in self.labels[i]:
                    found = False
                    break
            
            if found:
                self.displayed[i] = True
        
        self.axes.clear()
        self.button = CheckButtons(
            self.axes,
            [self.labels[i] for i in range(len(self.labels)) if self.displayed[i]],
            [self.states[i] for i in range(len(self.labels)) if self.displayed[i]],
        )
        self.button.on_clicked(self.process)
        self.axes.get_figure().canvas.draw_idle()
    
    def _process_maj_check(self, label1: str, label2: str):
        index1: int = self.labels.index(label1)
        index2: int = self.labels.index(label2)
        self.button.eventson = False
        for i in range(min(index1, index2), max(index1, index2) + 1):
            if (self.displayed[i]): self.check_index(i, self.states[index2])
        self.button.eventson = True
    def check_index(self, index: int, state: bool = True):
        if (self.displayed[index]):
            display_index: int = [i for i in range(len(self.displayed)) if self.displayed[i]].index(index)
            self.button.set_active(display_index, state)
        self.states[index] = state

class TextField:
    axes: plt.Axes
    field: TextBox
    
    def __init__(self, axes: plt.Axes, label: str, callback):
        self.axes = axes
        self.axes._is_button = True # custom tag
        self.field = TextBox(axes, label)
        self.field.on_submit(self.process)
        self.callback = callback
    
    def process(self, text):
        self.callback()
    
    def get_text(self)->str:
        return self.field.text
    
    def set_visible(self, state: bool):
        self.axes.set_visible(state)
    