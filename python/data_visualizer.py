import os
import sys
import ast
import pandas as pd
import numpy as np
import io

BUTTON_WIDTH: int = 0.2

if __name__ != "__main__":
    exit()
    
file_paths: str = ""
if (len(sys.argv) < 2):
    from file_selector import file_selector
    file_paths = file_selector()
else:
    file_paths = [sys.argv[1]]

# ===============================================================================================

import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from matplotlib.text import Annotation
from button import ButtonProcessor

# ===============================================================================================

class FileInfo:
    def __init__(self, path: str):
        self.name = os.path.basename(path)
        file_content: str = ""
        with open(path) as f: file_content = f.read()

        self.parameters = file_content.split("\n/*scores*/\n")[0]
        file_content = file_content.split("\n/*scores*/\n")[1]

        file_times = file_content.split("\n/*times*/\n")[1]
        file_content = file_content.split("\n/*times*/\n")[0]

        self.data = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[0]), sep = "\t", index_col = "generation")
        self.populations = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[1]), sep = "\t")
        self.times = pd.read_csv(io.StringIO(file_times), sep = "\t", index_col = "generation")
    
        self.global_labels = ["best_score", "gen_best_score", "mean_score", "std", "age"]
        self.local_labels = ["all_args"]
        self.times_labels = []
        self.args_labels = []
        
        for label in self.populations.columns.values:
            if (label not in ["generation", "age", "nb_args"]):
                self.args_labels.append(label)
                self.local_labels.append(label)
        for label in self.times.columns.values:
            if (label != "generation"):
                self.times_labels.append(label)

    def plot_global(self, fig: plt.Figure, ax: plt.Axes, plot_key: str, start_range_at_0: bool) -> tuple[list[plt.Line2D], list[str]]:
        if (plot_key == "age"):
            x = list(self.data.index)
            vc = self.populations[["generation", plot_key]].groupby("generation").mean()
            
            vc.plot.line(ax=ax)
        elif start_range_at_0:
            self.data.plot.line(y=plot_key, use_index=True, ax=ax, ylim=(-0.05, self.data[plot_key].max() + 0.1))
        else:
            self.data.plot.line(y=plot_key, use_index=True, ax=ax)
        return [], [self.name]
    def plot_time(self, fig: plt.Figure, ax: plt.Axes, plot_key: str, normalize_times: bool) -> tuple[list[plt.Line2D], list[str]]:
        if (normalize_times):
            (self.times[plot_key] / self.times.total).plot.line(y=plot_key, use_index=True, ax=ax, ylim=(-0.1, 1.1))
        else:
            self.times.plot.line(y=plot_key, use_index=True, ax=ax)
        return [], [self.name]
    def plot_arg(self, fig: plt.Figure, ax: plt.Axes, plot_key: str) -> tuple[list[plt.Line2D], list[str]]:
        x = list(self.data.index)
        vc = self.populations[["generation", plot_key]].groupby(plot_key).value_counts()
        result: list[plt.Line2D] = []
        
        if (len(vc) != 0):
            args, _ = zip(*vc.index.tolist())
            for arg in set(args):
                y = np.array(list(vc[arg].reindex(x, fill_value=0)))
                line, = ax.plot(x, y / np.array(self.populations[["generation"]].value_counts().tolist()), label=arg)
                result.append(line)
        return result, []
    def plot_all_args(self, fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
        x = list(self.data.index)
        y = {}
        
        for key in self.local_labels:
            if key == "all_args":
                continue
            
            vc = self.populations[["generation", key]].groupby(key).value_counts()
            if (len(vc) != 0):
                args, _ = zip(*vc.index.tolist())
                for arg in set(args):
                    if (arg not in y):
                        y[arg] = np.zeros(len(x))
                    y[arg] += np.array(list(vc[arg].reindex(x, fill_value=0)))
        
        result: list[plt.Line2D] = []
        for arg in y.keys():
            line, = ax.plot(x, y[arg] / np.array(self.populations[["generation"]].value_counts().tolist()), label=arg)
            result.append(line)
        return result, []
    
    def plot_data(self, fig: plt.Figure, ax: plt.Axes, plot_key: str, normalize_times: bool, start_range_at_0: bool) -> tuple[list[plt.Line2D], list[str]]:
        if (PLOT_KEY in self.global_labels):
            return self.plot_global(fig, ax, plot_key, start_range_at_0)
        elif (PLOT_KEY in self.times_labels):
            return self.plot_time(fig, ax, plot_key, normalize_times)
        elif (PLOT_KEY in self.args_labels):
            return self.plot_arg(fig, ax, plot_key)
        elif (PLOT_KEY == "all_args"):
            return self.plot_all_args(fig, ax)
        return [], []

file_infos: list[FileInfo] = [FileInfo(path) for path in file_paths]

for f in file_infos[0:]:
    if (
        f.args_labels != file_infos[0].args_labels or
        f.times_labels != file_infos[0].times_labels or
        f.local_labels != file_infos[0].local_labels or
        f.global_labels != file_infos[0].global_labels
        ):
        print("the file", f.path, "does not have the same labels as the file", file_infos[0].path)
        print("cannot run a good visualization")
        exit()

# ===============================================================================================

PLOT_CATEGORY = "global" # global / local / times
NORMALIZE_TIMES = False
RANGE_0 = False
PLOT_KEY = "best_score"

fig, ax = plt.subplots()
fig.subplots_adjust(left=0.1, right=0.9, bottom=0.35, top=0.95)
lines: list = []

annot: Annotation

def update(event):
    global lines, annot
    ax.clear()
    
    annot = ax.annotate("", xy=(0,0), xytext=(-20,20),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    lines = []
    legends = []
    for f in file_infos:
        line, legend = f.plot_data(fig, ax, PLOT_KEY, NORMALIZE_TIMES, RANGE_0)
        lines += line
        legends += legend
    
    if len(legends) != 0:
        ax.legend(legends)
    
    fig.canvas.draw_idle()

# region hovering hints
def update_annot(line_index: int, ind):
    x,y = lines[line_index].get_data()
    annot.xy = (x[ind["ind"][0]], y[ind["ind"][0]])
    annot.set_text(lines[line_index].get_label())
    annot.get_bbox_patch().set_alpha(0.4)

def hover(event):
    vis = annot.get_visible()
    if event.inaxes == ax:
        found: bool = False
        
        for index in range(len(lines)):
            cont, ind = lines[index].contains(event)
            if cont:
                found = True
                update_annot(index, ind)
                break

        if found or vis:
            annot.set_visible(found)
            fig.canvas.draw_idle()

fig.canvas.mpl_connect("motion_notify_event", hover)
# endregion

def plot_key_buttons_callback(value: str):
    global PLOT_KEY
    PLOT_KEY = value
    update(None)
# region local buttons
local_buttons_axes: list[plt.Axes] = []
local_buttons: list[ButtonProcessor] = []

if (len(file_infos) == 1):
    for l in file_infos[0].local_labels:
        local_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
        local_buttons.append(ButtonProcessor(local_buttons_axes[-1], l, plot_key_buttons_callback, l))

local_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
if (len(file_infos) == 1):
    local_slider = Slider(
        ax=local_slider_ax,
        label="slider",
        valmin=0,
        valmax=len(file_infos[0].local_labels) - 1,
        valinit=0,
        orientation="horizontal"
    )
    def local_slider_update(event):
        i = 0
        for a in local_buttons_axes:
            pos = a.get_position()
            pos.x0 = 0.1 - (local_slider.val - i) * (BUTTON_WIDTH + 0.025)
            pos.x1 = 0.1 + BUTTON_WIDTH - (local_slider.val - i) * (BUTTON_WIDTH + 0.025)
            i += 1
            a.set_position(pos)
        
        fig.canvas.draw_idle()
    local_slider.on_changed(local_slider_update)
    local_slider_update(None)

    def on_local_scroll(event):
        if (local_slider_ax.get_visible()):
            increment = 1 if event.button == 'up' else -1
            local_slider.set_val(max(local_slider.valmin, min(local_slider.valmax, local_slider.val + increment * 0.5)))
            local_slider_update(None)

    fig.canvas.mpl_connect('scroll_event', on_local_scroll)
# endregion
# region global buttons
range_0_button_axe = fig.add_axes([0.45, 0.2, 0.3, 0.05])
range_0_button = Button(range_0_button_axe, "start y at 0: false")
def range_0_buttons_callback(event):
    global RANGE_0
    RANGE_0 = not(RANGE_0)
    
    range_0_button.label.set_text("start y at 0: true" if RANGE_0 else "start y at 0: false")
    
    update(None)
range_0_button.on_clicked(range_0_buttons_callback)

global_buttons_axes: list[plt.Axes] = []
global_buttons: list[ButtonProcessor] = []
for l in file_infos[-1].global_labels:
    global_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    global_buttons.append(ButtonProcessor(global_buttons_axes[-1], l, plot_key_buttons_callback, l))

global_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
global_slider = Slider(
    ax=global_slider_ax,
    label="slider",
    valmin=0,
    valmax=len(file_infos[-1].global_labels) - 1,
    valinit=0,
    orientation="horizontal"
)
def global_slider_update(event):
    i = 0
    for a in global_buttons_axes:
        pos = a.get_position()
        pos.x0 = 0.1 - (global_slider.val - i) * (BUTTON_WIDTH + 0.025)
        pos.x1 = 0.1 + BUTTON_WIDTH - (global_slider.val - i) * (BUTTON_WIDTH + 0.025)
        i += 1
        a.set_position(pos)
    
    fig.canvas.draw_idle()
global_slider.on_changed(global_slider_update)
global_slider_update(None)

def on_global_scroll(event):
    if (global_slider_ax.get_visible()):
        increment = 1 if event.button == 'up' else -1
        global_slider.set_val(max(global_slider.valmin, min(global_slider.valmax, global_slider.val + increment * 0.5)))
        global_slider_update(None)

fig.canvas.mpl_connect('scroll_event', on_global_scroll)
# endregion
# region times buttons
normalize_button_axe = fig.add_axes([0.45, 0.2, 0.3, 0.05])
normalize_button = Button(normalize_button_axe, "normalize: false")
def normalize_buttons_callback(event):
    global NORMALIZE_TIMES
    NORMALIZE_TIMES = not(NORMALIZE_TIMES)
    
    normalize_button.label.set_text("normalize: true" if NORMALIZE_TIMES else "normalize: false")
    
    update(None)
normalize_button.on_clicked(normalize_buttons_callback)

times_buttons_axes: list[plt.Axes] = []
times_buttons: list[ButtonProcessor] = []
for l in file_infos[-1].times_labels:
    times_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    times_buttons.append(ButtonProcessor(times_buttons_axes[-1], l, plot_key_buttons_callback, l))

times_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
times_slider = Slider(
    ax=times_slider_ax,
    label="slider",
    valmin=0,
    valmax=len(file_infos[-1].times_labels) - 1,
    valinit=0,
    orientation="horizontal"
)
def times_slider_update(event):
    i = 0
    for a in times_buttons_axes:
        pos = a.get_position()
        pos.x0 = 0.1 - (times_slider.val - i) * (BUTTON_WIDTH + 0.025)
        pos.x1 = 0.1 + BUTTON_WIDTH - (times_slider.val - i) * (BUTTON_WIDTH + 0.025)
        i += 1
        a.set_position(pos)
    
    fig.canvas.draw_idle()
times_slider.on_changed(times_slider_update)
times_slider_update(None)

def on_times_scroll(event):
    if (times_slider_ax.get_visible()):
        increment = 1 if event.button == 'up' else -1
        times_slider.set_val(max(times_slider.valmin, min(times_slider.valmax, times_slider.val + increment * 0.5)))
        times_slider_update(None)

fig.canvas.mpl_connect('scroll_event', on_times_scroll)
# endregion

# region category button
category_ax = fig.add_axes([0.1, 0.2, 0.3, 0.05])
category_button = Button(category_ax, PLOT_CATEGORY)

def update_visibility():
    for a in local_buttons_axes + [local_slider_ax]:
        a.set_visible(PLOT_CATEGORY == "individual")
    for a in global_buttons_axes + [global_slider_ax, range_0_button_axe]:
        a.set_visible(PLOT_CATEGORY == "global")
    for a in times_buttons_axes + [times_slider_ax, normalize_button_axe]:
        a.set_visible(PLOT_CATEGORY == "times")
    fig.canvas.draw_idle()
def switch_category(event):
    global PLOT_CATEGORY
    PLOT_CATEGORY = "global" if (PLOT_CATEGORY == "times") else "individual" if (PLOT_CATEGORY == "global") else "times"
    category_button.label.set_text(PLOT_CATEGORY)
    
    update_visibility()
update_visibility()
category_button.on_clicked(switch_category)
# endregion

update(None)
plt.show()