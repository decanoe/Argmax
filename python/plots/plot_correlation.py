from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib.backend_bases import Event

from data_loader import DataLoader, NKRunFile
from plots.plot_annoted_axis import PlotAnnotedAxis

class PlotCorrelation(PlotAnnotedAxis):
    def __init__(self, window: Window, axis: plt.Axes = None):
        if (axis == None):
            axis = window.figure.add_subplot(111)
        super().__init__(window, axis)
        
        self.prepare_plot()
        self.plot()
    
    def prepare_plot(self):
        self.axis.set_xlabel(self.window.get_axis1_label())
        self.axis.set_ylabel(self.window.get_axis2_label())
        self.axis.grid()
    def plot(self):
        max_xy: float = 0
        for algo in self.window.get_selected_algos():
            if algo.startswith("hc_"):
                continue
            
            all_x, all_y = self.get_single_correlation_points(algo)
            max_xy = max(max_xy, all_x.max(), all_y.max())
            
            self.plot_single(algo, all_x, all_y)
        
        if self.do_plot_x_equal_y_reference():
            line, = self.axis.plot(np.linspace(0, max_xy), np.linspace(0, max_xy), color = "gray", linestyle = "--", label = "x = y")
            self.add_line(line, "x=y")
    def get_single_correlation_points(self, algo: str) -> tuple[np.ndarray[float], np.ndarray[float]]:
        axis1: str = self.window.get_axis1()
        axis2: str = self.window.get_axis2()
        data_loader: DataLoader = self.window.get_data_loader()
        infos: NKRunFile = data_loader.get_file(algo)
        
        all_x: np.ndarray[float] = infos.get_jumps(axis1.replace("_delta", "_after_jump"))
        if (axis1.endswith("_delta")):
            all_x -= infos.get_jumps(axis1.replace("_delta", "_before_jump"))
        
        all_y: np.ndarray[float] = infos.get_jumps(axis2.replace("_delta", "_after_jump"))
        if (axis2.endswith("_delta")):
            all_y -= infos.get_jumps(axis2.replace("_delta", "_before_jump"))
        
        return all_x, all_y
    
    def do_plot_x_equal_y_reference(self) -> bool:
        axis1: str = self.window.get_axis1()
        axis2: str = self.window.get_axis2()
        return not("fitness" in axis1 + axis2) and not("delta" in axis1 + axis2)
    def plot_single(self, algo: str, all_x: np.ndarray[float], all_y: np.ndarray[float]):
        pass