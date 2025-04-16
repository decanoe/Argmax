import os
import sys
import ast
import pandas as pd
import numpy as np
import io

BUTTON_WIDTH: int = 0.2

if __name__ != "__main__":
    exit()
    
data_file: str = ""
if (len(sys.argv) < 2):
    from file_selector import file_selector
    data_file = file_selector()
else:
    data_file = sys.argv[1]

file_content: str = ""
with open(data_file) as f: file_content = f.read()

parameters = file_content.split("\n/*scores*/\n")[0]
file_content = file_content.split("\n/*scores*/\n")[1]

file_times = file_content.split("\n/*times*/\n")[1]
file_content = file_content.split("\n/*times*/\n")[0]

data = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[0]), sep = "\t", index_col = "generation")
populations = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[1]), sep = "\t")
times = pd.read_csv(io.StringIO(file_times), sep = "\t", index_col = "generation")

global_labels = ["best_score", "gen_best_score", "mean_score", "std", "age"]
local_labels = ["all_args"]
times_labels = []
args_labels = []

for label in populations.columns.values:
    if (label not in ["generation", "age", "nb_args"]):
        args_labels.append(label)
        local_labels.append(label)
for label in times.columns.values:
    if (label != "generation"):
        times_labels.append(label)
 
# ===============================================================================================
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from matplotlib.text import Annotation
from button import ButtonProcessor

PLOT_CATEGORY = "global" # global / individual / times
NORMALIZE_TIMES = False
PLOT_KEY = "best_score"

fig, ax = plt.subplots()
fig.subplots_adjust(left=0.1, right=0.9, bottom=0.35, top=0.95)
lines: list = []

annot: Annotation

def update(event):
    global lines, annot
    lines = []
    ax.clear()
    
    annot = ax.annotate("", xy=(0,0), xytext=(-20,20),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    if (PLOT_KEY == "age"):
        x = list(data.index)
        vc = populations[["generation", PLOT_KEY]].groupby("generation").mean()
        
        vc.plot.line(ax=ax)
    elif (PLOT_KEY in global_labels):
        data.plot.line(y=PLOT_KEY, use_index=True, ax=ax)
    elif (PLOT_KEY in times_labels):
        if (NORMALIZE_TIMES):
            (times[PLOT_KEY] / times.total).plot.line(y=PLOT_KEY, use_index=True, ax=ax, ylim=(-0.1, 1.1))
        else:
            times.plot.line(y=PLOT_KEY, use_index=True, ax=ax)
    elif (PLOT_KEY in args_labels):
        x = list(data.index)
        vc = populations[["generation", PLOT_KEY]].groupby(PLOT_KEY).value_counts()
        
        if (len(vc) != 0):
            args, _ = zip(*vc.index.tolist())
            for arg in set(args):
                y = np.array(list(vc[arg].reindex(x, fill_value=0)))
                line, = ax.plot(x, y / np.array(populations[["generation"]].value_counts().tolist()), label=arg)
                lines.append(line)
    elif (PLOT_KEY == "all_args"):
        x = list(data.index)
        y = {}
        
        for key in local_labels:
            if key == "all_args":
                continue
            
            vc = populations[["generation", key]].groupby(key).value_counts()
            if (len(vc) != 0):
                args, _ = zip(*vc.index.tolist())
                for arg in set(args):
                    if (arg not in y):
                        y[arg] = np.zeros(len(x))
                    y[arg] += np.array(list(vc[arg].reindex(x, fill_value=0)))
        
        for arg in y.keys():
            line, = ax.plot(x, y[arg] / np.array(populations[["generation"]].value_counts().tolist()), label=arg)
            lines.append(line)
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
for l in local_labels:
    local_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    local_buttons.append(ButtonProcessor(local_buttons_axes[-1], l, plot_key_buttons_callback, l))

local_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
local_slider = Slider(
    ax=local_slider_ax,
    label="slider",
    valmin=0,
    valmax=len(local_labels) - 1,
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
global_buttons_axes: list[plt.Axes] = []
global_buttons: list[ButtonProcessor] = []
for l in global_labels:
    global_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    global_buttons.append(ButtonProcessor(global_buttons_axes[-1], l, plot_key_buttons_callback, l))

global_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
global_slider = Slider(
    ax=global_slider_ax,
    label="slider",
    valmin=0,
    valmax=len(global_labels) - 1,
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
for l in times_labels:
    times_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    times_buttons.append(ButtonProcessor(times_buttons_axes[-1], l, plot_key_buttons_callback, l))

times_slider_ax = fig.add_axes([0.1, 0, 0.8, 0.05])
times_slider = Slider(
    ax=times_slider_ax,
    label="slider",
    valmin=0,
    valmax=len(times_labels) - 1,
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
    for a in global_buttons_axes + [global_slider_ax]:
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