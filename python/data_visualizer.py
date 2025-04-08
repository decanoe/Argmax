import os
import sys
import ast
import pandas as pd
import io

if __name__ != "__main__":
    exit()
    
data_file: str = ""
if (len(sys.argv) < 2):
    from file_selector import file_selector
    data_file = file_selector()
else:
    data_file = sys.argv[1]

problem: str = os.path.basename(data_file).split("_")[0]
print(problem)

file_content: str = ""
with open(data_file) as f: file_content = f.read()

data = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[0]), sep = "\t", index_col = "generation")
populations = pd.read_csv(io.StringIO(file_content.split("/*populations*/\n")[1]), sep = "\t", index_col = "generation")





# ===============================================================================================
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from button import ButtonProcessor

PLOT_CATEGORY = "global" # global / individual
USE_ARG_ORDER = True
PLOT_KEY = "best_score"

fig, ax = plt.subplots()
fig.subplots_adjust(left=0.1, right=0.9, bottom=0.35, top=0.95)

global_labels = ["best_score", "gen_best_score", "std"]
local_labels = ["all"]
if (problem == "FA"):
    local_labels += [f"people{i}" for i in range(1, 9)] + [f"sanct{i}" for i in range(1, 8)]

def update(event):
    ax.clear()
    
    if (PLOT_KEY in global_labels):
        data.plot.line(y=PLOT_KEY, use_index=True, ax=ax)
    else:
        pass
    fig.canvas.draw_idle()

# region local buttons
def local_buttons_callback(value: str):
    global PLOT_KEY
    PLOT_KEY = value
    update(None)

local_buttons_axes: list[plt.Axes] = []
local_buttons: list[ButtonProcessor] = []
for l in local_labels:
    local_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    local_buttons.append(ButtonProcessor(local_buttons_axes[-1], l, local_buttons_callback, l))

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
        pos.x0 = 0.1 - (local_slider.val - i) * 0.35
        pos.x1 = 0.4 - (local_slider.val - i) * 0.35
        i += 1
        a.set_position(pos)
    
    fig.canvas.draw_idle()
local_slider.on_changed(local_slider_update)
local_slider_update(None)
# endregion
# region global buttons
def global_buttons_callback(value: str):
    global PLOT_KEY
    PLOT_KEY = value
    update(None)

global_buttons_axes: list[plt.Axes] = []
global_buttons: list[ButtonProcessor] = []
for l in global_labels:
    global_buttons_axes.append(fig.add_axes([0, 0.1, 0, 0.05]))
    global_buttons.append(ButtonProcessor(global_buttons_axes[-1], l, global_buttons_callback, l))

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
        pos.x0 = 0.1 - (global_slider.val - i) * 0.35
        pos.x1 = 0.4 - (global_slider.val - i) * 0.35
        i += 1
        a.set_position(pos)
    
    fig.canvas.draw_idle()
global_slider.on_changed(global_slider_update)
global_slider_update(None)
# endregion

# region arg_order button
arg_order_ax = fig.add_axes([0.45, 0.2, 0.3, 0.05])
arg_order_button = Button(arg_order_ax, "arg_order: on" if USE_ARG_ORDER else "arg_order: off")

def switch_arg_order(event):
    global USE_ARG_ORDER
    USE_ARG_ORDER = not(USE_ARG_ORDER)
    arg_order_button.label.set_text("arg_order: on" if USE_ARG_ORDER else "arg_order: off")
    update(event)
# endregion

# region category button
category_ax = fig.add_axes([0.1, 0.2, 0.3, 0.05])
category_button = Button(category_ax, PLOT_CATEGORY)

def update_visibility():
    for a in local_buttons_axes + [arg_order_ax, local_slider_ax]:
        a.set_visible(PLOT_CATEGORY == "individual")
    for a in global_buttons_axes + [global_slider_ax]:
        a.set_visible(PLOT_CATEGORY == "global")
def switch_category(event):
    global PLOT_CATEGORY
    PLOT_CATEGORY = "global" if (PLOT_CATEGORY != "global") else "individual"
    category_button.label.set_text(PLOT_CATEGORY)
    
    update_visibility()
    
    update(event)
update_visibility()
# endregion

# region button events
category_button.on_clicked(switch_category)
arg_order_button.on_clicked(switch_arg_order)
# endregion

update(None)
plt.show()