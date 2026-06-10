from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

from matplotlib.axes import Axes
from matplotlib.backend_bases import Event

from plots.plot_axis import PlotAxis
from plots.plot_correlation import PlotCorrelation
from plots.plot_correlation_hist import PlotCorrelationHistogram
from plots.plot_correlation_reg import PlotCorrelationReg
from plots.plot_correlation_mean_std import PlotCorrelationMeanStd

class PlotDualCorrelation(PlotAxis):
    correlation: PlotCorrelation
    histogram: PlotCorrelation
    
    def __init__(self, window: Window):
        super().__init__(window)
        
        axis: Axes = None
        if (window.do_hist_plot()):
            axis = window.figure.add_subplot(211)
        else:
            axis = window.figure.add_subplot(111)
        
        if (self.window.get_correlation_type() == "regression"):
            self.correlation = PlotCorrelationReg(window, axis)
        else:
            self.correlation = PlotCorrelationMeanStd(window, axis)
        
        if (window.do_hist_plot()):
            self.histogram = PlotCorrelationHistogram(window, window.figure.add_subplot(212, sharex=self.correlation.axis))
    
    def soft_update(self, event: Event = None):
        super().soft_update(event)
        self.correlation.soft_update(event)
        self.histogram.soft_update(event)
