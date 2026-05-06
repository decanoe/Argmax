from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

import matplotlib.pyplot as plt
from matplotlib.backend_bases import Event

from plots.plot_annoted_axis import PlotAnnotedAxis
from data_loader import DataLoader

class PlotOneRun(PlotAnnotedAxis):
    def __init__(self, window: Window, axis: plt.Axes = None):
        if (axis == None):
            axis = window.figure.add_subplot(111)
        super().__init__(window, axis)
        self.prepare_plot()
        self.plot()
    
    def soft_update(self, event: Event = None):
        super().soft_update(event)
        if (self.window.get_x_scale() == "log scale"):
            self.axis.set_xscale('log')
        else:
            self.axis.set_xscale('linear')
    
    def prepare_plot(self):
        if (self.window.get_x_scale() == "log scale"):
            self.axis.set_xscale('log')
        
        self.axis.set_xlabel("budget")
        self.axis.set_ylabel("fitness")
        self.axis.grid()
    def plot(self):
        data_loader: DataLoader = self.window.get_data_loader()
        
        sorted_algos = sorted(self.window.get_selected_algos(), key=lambda algo:data_loader.get_file(algo).get_one_run_scores().fitness.max(), reverse=True)
        
        for algo in sorted_algos:
            run_info = data_loader.get_file(algo)
            data = run_info.get_one_run_scores()
            
            line, = self.axis.plot(data.budget, data.fitness, label=run_info.label, color = run_info.color, linestyle = run_info.linestyle)
            self.add_line(line, run_info.label)
