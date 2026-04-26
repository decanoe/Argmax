from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

import matplotlib.pyplot as plt
from matplotlib.backend_bases import Event

from plots.plot_annoted_axis import PlotAnnotedAxis
from data_loader import DataLoader

class PlotAnytime(PlotAnnotedAxis):
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
        
        sorted_algos = sorted(data_loader.Algo_keys, key=lambda algo:data_loader.get_file(self.window.get_n(), self.window.get_k(), algo, self.window.get_i()).get_data().fitness_after_jump.max(), reverse=True)
        
        for algo in sorted_algos:
            if not(self.window.is_algo_selected(algo)):
                continue
            
            run_info = data_loader.get_file(self.window.get_n(), self.window.get_k(), algo, self.window.get_i())
            data = run_info.get_data()
            
            all_x = []
            all_y = []
            
            current_x = 0
            temp = data
            while len(temp) != 0:
                current_x = temp.budget.iloc[0]
                all_x.append(current_x)
                all_y.append(temp.fitness_after_jump.iloc[0])
                temp = data[data.fitness_after_jump > all_y[-1]]
            
            max_x = data.budget.max()
            if (current_x != max_x):
                all_x.append(max_x)
                all_y.append(all_y[-1])
            
            line, = self.axis.plot(all_x, all_y, label=run_info.label, color = run_info.color, linestyle = run_info.linestyle)
            self.add_line(line, run_info.label)
