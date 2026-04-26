from __future__ import annotations
from typing import TYPE_CHECKING

import numpy as np
if TYPE_CHECKING:
    from window import Window

from matplotlib import pyplot as plt

from plots.plot_correlation import PlotCorrelation

class PlotCorrelationMeanStd(PlotCorrelation):
    def __init__(self, window: Window, axis: plt.Axes = None):
        super().__init__(window, axis)

    def plot(self):
        if (self.window.get_axis1().startswith("fitness")):
            return # can't plot with non discrete values for the x axis
        super().plot()
    def plot_single(self, algo: str, all_x: np.ndarray[float], all_y: np.ndarray[float]):
        some_run_info = self.window.get_data_loader().get_reference_file(algo)
        
        discrete_values: np.ndarray[float] = np.unique(all_x)
        mean: np.ndarray[float] = np.zeros(discrete_values.size)
        std: np.ndarray[float] = np.zeros(discrete_values.size)
        
        for i in range(discrete_values.size):
            mask = (all_x == discrete_values[i])
            mean[i] = all_y[mask].mean()
            std[i] = all_y[mask].std()
        
        self.axis.fill_between(discrete_values, mean - std, mean + std, alpha=.25, linewidth=0, color = some_run_info.color)
        line, = self.axis.plot(discrete_values, mean, color = some_run_info.color, linestyle = some_run_info.linestyle, label=some_run_info.label + " mean")
        self.add_line(line, some_run_info.label + " mean")