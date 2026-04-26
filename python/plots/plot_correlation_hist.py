from __future__ import annotations
from typing import TYPE_CHECKING

from matplotlib.backend_bases import Event
import numpy as np
if TYPE_CHECKING:
    from window import Window

from matplotlib import pyplot as plt

from plots.plot_correlation import PlotCorrelation

class PlotCorrelationHistogram(PlotCorrelation):
    def __init__(self, window: Window, axis: plt.Axes = None):
        super().__init__(window, axis)

    def prepare_plot(self):
        super().prepare_plot()
        self.axis.set_ylabel("frequency")
    def plot(self):
        if (self.window.get_axis1().startswith("fitness")):
            return # can't plot with non discrete values for the x axis
        super().plot()
        
        if (self.axis.get_legend() is not None):
            self.axis.get_legend().remove()
    def soft_update(self, event: Event = None):
        super().soft_update(event)
        
        if (self.axis.get_legend() is not None):
            self.axis.get_legend().remove()

    def do_plot_x_equal_y_reference(self):
        return False
    def plot_single(self, algo: str, all_x: np.ndarray[float], all_y: np.ndarray[float]):
        some_run_info = self.window.get_data_loader().get_reference_file(algo)
        
        discrete_values: np.ndarray[float] = np.unique(all_x)
        counts: np.ndarray[float] = np.zeros(discrete_values.size)
        
        for i in range(discrete_values.size):
            mask = (all_x == discrete_values[i])
            counts[i] = mask.sum()
        
        counts /= counts.sum()
        
        line, = self.axis.plot(discrete_values, counts, color = some_run_info.color, linestyle = some_run_info.linestyle, label=some_run_info.label + " frequency")
        self.add_line(line, some_run_info.label + " frequency")