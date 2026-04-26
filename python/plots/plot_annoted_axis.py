from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from window import Window

import matplotlib.pyplot as plt
from matplotlib.backend_bases import Event
from matplotlib.text import Annotation
from plots.plot_axis import PlotAxis

class PlotAnnotedAxis(PlotAxis):
    axis: plt.Axes
    annotation: Annotation
    lines: list[plt.Line2D]
    legends: list[str]
    
    active_annotation: bool
    last_hovered_line: plt.Line2D
    legend_position: str
    
    def __init__(self, window: Window, axis: plt.Axes):
        super().__init__(window)
        self.axis = axis
        self.annotation = self.axis.annotate("", xy=(0,0), xytext=(-20,20),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
        self.annotation.set_visible(False)
        self.active_annotation = not(window.is_full_screen())
        self.last_hovered_line = None
        self.legend_position = window.get_legend_position()
        self.lines = []
        self.legends = []
        
        self.axis.get_figure().canvas.mpl_connect("motion_notify_event", self.hover)
    
    def soft_update(self, event: Event = None):
        super().soft_update(event)
        self.set_legend_position(self.window.get_legend_position())
        self.set_annotation_active(not(self.window.is_full_screen()))
    
    def set_legend_position(self, legend_position: str):
        if (self.legend_position == legend_position):
            return
        self.legend_position = legend_position
        self.draw_legend()
    def set_annotation_active(self, state: bool):
        self.active_annotation = state
        if (not(self.active_annotation) and self.annotation.get_visible()):
            self.annotation.set_visible(False)
            self.axis.get_figure().canvas.draw_idle()
            self.last_hovered_line = None
    def add_lines(self, lines: list[plt.Line2D], legends: list[str] = []):
        self.lines += lines
        self.legends += legends
        self.draw_legend()
    def add_line(self, line: plt.Line2D, legend: str = ""):
        self.lines.append(line)
        self.legends.append(legend)
        self.draw_legend()
    def draw_legend(self):
        if len(self.legends) != 0:
            leg = self.axis.legend(
                [self.lines[i] for i in range(len(self.legends)) if self.legends[i] != ""],
                [l for l in self.legends if l != ""],
                loc=self.legend_position
                )
            for lh in leg.legend_handles:
                lh.set_alpha(1)
    
    def update_annot(self, line: plt.Line2D, ind: dict):
        x,y = line.get_data()
        self.annotation.xy = (x[ind["ind"][0]], y[ind["ind"][0]])
        self.annotation.set_text(line.get_label())
        self.annotation.get_bbox_patch().set_alpha(0.4)
    def is_over_line(self, event: Event, line: plt.Line2D) -> tuple[bool, dict]:
        if (line == None):
            return (False, {})
        
        # Fast bbox check
        if not line.get_path().get_extents().contains(event.xdata, event.ydata):
            return (False, {})
        return line.contains(event)
    def hover(self, event: Event):
        vis = self.annotation.get_visible()
        if not(self.active_annotation):
            if (vis):
                self.annotation.set_visible(False)
                self.axis.get_figure().canvas.draw_idle()
                self.last_hovered_line = None
            return
        
        if event.inaxes == self.axis:
            found: bool = False
            
            # check previous line
            if (self.last_hovered_line != None):
                state, ind = self.is_over_line(event, self.last_hovered_line)
                
                if (state):
                    found = True
                    self.update_annot(self.last_hovered_line, ind)
            
            # check all other lines
            if not(found):
                self.last_hovered_line = None
                for line in self.lines:
                    state, ind = self.is_over_line(event, line)
                    
                    if (state):
                        found = True
                        self.update_annot(line, ind)
                        self.last_hovered_line = line
                        break

            if found or vis:
                self.annotation.set_visible(found)
                self.axis.get_figure().canvas.draw_idle()