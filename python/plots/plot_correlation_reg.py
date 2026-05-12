from __future__ import annotations
from typing import TYPE_CHECKING

import numpy as np
if TYPE_CHECKING:
    from window import Window

from matplotlib import pyplot as plt

from plots.plot_correlation import PlotCorrelation

class PlotCorrelationReg(PlotCorrelation):
    def __init__(self, window: Window, axis: plt.Axes = None):
        super().__init__(window, axis)

    def plot_single(self, algo: str, all_x: np.ndarray[float], all_y: np.ndarray[float]):
        run_info = self.window.get_data_loader().get_file(algo)
        p, = self.axis.plot(all_x, all_y, linestyle="", marker='o', color = run_info.algo_infos.get_plot_color(), markersize=3, label=run_info.algo_infos.get_plot_label(), alpha = min(1, 150 / len(all_x)))
        self.add_line(p)
        
        #region aproximation
        theta = np.polyfit(all_x, all_y, deg=self.window.get_regression_degree())
        model = np.poly1d(theta)
        x = np.linspace(all_x.min(), all_x.max(), 100)
        line, = self.axis.plot(x, model(x), label=run_info.algo_infos.get_plot_label() + " regression", color = run_info.algo_infos.get_plot_color(), linestyle = run_info.algo_infos.get_plot_style())
        self.add_line(line, run_info.algo_infos.get_plot_label() + " regression")
        #endregion