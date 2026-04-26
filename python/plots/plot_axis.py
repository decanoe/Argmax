from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window
    
from matplotlib.backend_bases import Event

class PlotAxis:
    window: Window
    
    def __init__(self, window: Window):
        self.window = window
    
    def soft_update(self, event: Event = None):
        pass