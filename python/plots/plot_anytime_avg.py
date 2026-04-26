from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

from matplotlib import pyplot as plt
import numpy as np

from plots.plot_anytime import PlotAnytime
from data_loader import DataLoader

class PlotAnytimeAvg(PlotAnytime):
    
    def __init__(self, window: Window, axis: plt.Axes = None):
        super().__init__(window, axis)
    
    def plot(self):
        data_loader: DataLoader = self.window.get_data_loader()
        
        all_points_per_algo: dict[str, tuple[np.ndarray[int], np.ndarray[float]]] = { algo: data_loader.get_avg_points(self.window.get_n(), self.window.get_k(), algo) for algo in data_loader.Algo_keys if self.window.is_algo_selected(algo) }

        sorted_algos = sorted(all_points_per_algo.keys(), key=lambda algo: all_points_per_algo[algo][1][-1], reverse=True)
        for algo in sorted_algos:
            x, y = all_points_per_algo[algo]
            
            some_run_info = data_loader.get_reference_file(algo)
            line, = self.axis.plot(x, y, label=some_run_info.label, color = some_run_info.color, linestyle = some_run_info.linestyle)
            self.add_line(line, some_run_info.label)
        
