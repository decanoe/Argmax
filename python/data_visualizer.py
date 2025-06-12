import os
import sys
import pandas as pd
import numpy as np
import io

dir_path = os.path.dirname(os.path.realpath(__file__))

if __name__ != "__main__":
    exit()

# ===============================================================================================

import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from matplotlib.text import Annotation
from button import ButtonCycle, ButtonCheck

DATA_TYPE: ButtonCycle = None
PLOT_TYPE: ButtonCycle = None
N: ButtonCycle = None
K: ButtonCycle = None
I: ButtonCycle = None
SELECTED_ALGOS: ButtonCheck = None
AXIS1: ButtonCycle = None
AXIS2: ButtonCycle = None

ALGO_KEYS: list[str] = []

# ===============================================================================================

class NKRunInfo:
    data: pd.DataFrame
    name: str
    
    N: int
    K: int
    algo: str
    seed: int
    instance: int
    
    def __init__(self, path: str):
        self.name = os.path.basename(path)
        with open(path) as f: self.data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
        
        temp = self.name.removesuffix(".rundata").split("_")
        self.algo, self.instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        self.K = int(os.path.basename(dir).removeprefix("K"))
        dir = os.path.dirname(dir)
        self.N = int(os.path.basename(dir).removeprefix("N"))
    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"
NK_file_infos: dict[int, dict[int, dict[str, dict[int, NKRunInfo]]]] = {}

from tqdm import tqdm
dirs = list(os.walk(dir_path+"\\data\\local_search\\NK"))
for i in tqdm(range(len(dirs))):
    for f in dirs[i][2]:
        info: NKRunInfo = NKRunInfo(dirs[i][0] + "\\" + f)
        NK_file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, {})[info.instance] = info

N_keys = sorted(NK_file_infos.keys())
K_keys = sorted(NK_file_infos[N_keys[0]].keys())
ALGO_KEYS = sorted(NK_file_infos[N_keys[0]][K_keys[0]].keys())
ALGO_COLORS = { ALGO_KEYS[i]: plt.cm.rainbow(np.linspace(0, 1, len(ALGO_KEYS)))[i] for i in range(len(ALGO_KEYS)) }

def NK_plot_anytime(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    
    colors = []
    last_x = []
    last_y = []
    max_x = 0
    
    sorted_algos = sorted(ALGO_KEYS, key=lambda algo:NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()].data.fitness.max(), reverse=True)
    
    for algo in sorted_algos:
        data = NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()].data
        
        all_x = []
        all_y = []
        
        current_x = 0
        temp = data
        while len(temp) != 0:
            current_x = temp.budget.iloc[0]
            all_x.append(current_x)
            all_y.append(temp.fitness.iloc[0])
            temp = data[data.fitness > all_y[-1]]
        
        max_x = max(max_x, current_x)
        
        line, = ax.plot(all_x, all_y, label=algo, color = ALGO_COLORS[algo])
        legends.append(algo)
        lines.append(line)
        
        colors.append(line.get_color())
        last_x.append(all_x[-1])
        last_y.append(all_y[-1])
    
    for i in range(len(last_x)):
        ax.plot([last_x[i], max_x], [last_y[i], last_y[i]], color=colors[i], linestyle='dashed')
    
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    return lines, legends
def NK_plot_correlation(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    legends = []
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        if not(SELECTED_ALGOS.button.get_status()[i]):
            continue
         
        info = NK_file_infos[N.get_value()][K.get_value()][ALGO_KEYS[i]][I.get_value()]
        
        all_x = list(info.data[info.data.size_of_the_jump != 0][AXIS1.get_value()].to_numpy())
        all_y = list(info.data[info.data.size_of_the_jump != 0][AXIS2.get_value()].to_numpy())
        
        max_val = max(max_val, info.data[info.data.size_of_the_jump != 0][AXIS1.get_value()].max(), info.data[info.data.size_of_the_jump != 0][AXIS2.get_value()].max())
        
        ax.scatter(all_x, all_y, label=ALGO_KEYS[i], s=5)
        legends.append(ALGO_KEYS[i])
    
    if (AXIS1.get_value() in ["nb_better_neighbors", "size_of_the_jump"] and AXIS2.get_value() in ["nb_better_neighbors", "size_of_the_jump"]):
        ax.plot([0, max_val], [0, max_val], color = "black")
    
    ax.set_xlabel(AXIS1.get_label())
    ax.set_ylabel(AXIS2.get_label())
    return [], legends
def NK_plot(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    if (PLOT_TYPE.get_value() == "anytime"):
        return NK_plot_anytime(fig, ax)
    return NK_plot_correlation(fig, ax)

# region output_table
def NK_generate_table():
    avg_content: str = "inst" + "\t".join(ALGO_KEYS)
    budgets: list[int] = [100000, 50000, 10000, 5000, 1000]
    best_budget_content: list[str] = [avg_content for _ in budgets]
    
    for n, n_data in NK_file_infos.items():
        for k, k_data in n_data.items():
            # region avg
            avg_content += f"\n{n}_{k}"
            for algo, algo_data in k_data.items():
                avg: float = 0
                count: int = 0
                for i_data in algo_data.values():
                    for s_data in i_data.values():
                        avg += float(s_data.data.fitness.iloc[-1])
                        count += 1
                avg_content += f"\t{round(avg/count, 6)}"
            # endregion
            
            # region best_budget_content
            for budget_index in range(len(budgets)):
                best_budget_content[budget_index] += f"\n{n}_{k}"
                for algo, algo_data in k_data.items():
                    avg: float = 0
                    count: int = 0
                    for i_data in algo_data.values():
                        budget: int = budgets[budget_index]
                        max_fitness: float = 0
                        for s_data in i_data.values():
                            for index in range(-1, -len(s_data.data), -1):
                                b: int = s_data.data.budget.iloc[index]
                                if (b <= budget):
                                    max_fitness = max(max_fitness, s_data.data.fitness.iloc[index])
                                    budget -= b
                                    break
                        avg += max_fitness
                        count += 1
                    best_budget_content[budget_index] += f"\t{round(avg/count, 6)}"
            # endregion
    
    with open(dir_path + "/output/avg.txt", "w") as f: f.write(avg_content)
    for budget_index in range(len(budgets)):
        with open(dir_path + f"/output/best_{budgets[budget_index]//1000}_000.txt", "w") as f: f.write(best_budget_content[budget_index])

# NK_generate_table()
# endregion

# ===============================================================================================
fig, ax = plt.subplots()
fig.subplots_adjust(left=0.1, right=0.9, bottom=0.35, top=0.95)
lines: list = []
annot: Annotation

def plot(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    if (DATA_TYPE.get_value() == "NK"):
        return NK_plot(fig, ax)
def update(event = None):
    global lines, annot
    ax.clear()
    
    annot = ax.annotate("", xy=(0,0), xytext=(-20,20),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    lines, legends = plot(fig, ax)
    
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

# region NK =====================================================================================
def update_NK():
    global N, K, I, SELECTED_ALGOS
    N_keys = sorted(NK_file_infos.keys())
    K_keys = sorted(NK_file_infos[N_keys[0]].keys())
    I_keys = sorted(NK_file_infos[N_keys[0]][K_keys[0]][ALGO_KEYS[0]].keys())
    
    N = ButtonCycle(fig.add_axes([0.1, 0.14, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, update)
    K = ButtonCycle(fig.add_axes([0.21, 0.14, 0.1, 0.05]), ["K" + str(i) for i in K_keys], K_keys, update)
    I = ButtonCycle(fig.add_axes([0.32, 0.14, 0.1, 0.05]), ["I" + str(i) for i in I_keys], I_keys, update)
# endregion =====================================================================================

# region permanent buttons ======================================================================
def update_visibility():
    if (DATA_TYPE.get_value() == "NK"):
        update_NK()
    
    for b in [K]:
        b.set_visible(DATA_TYPE.get_value() == "NK")
    for b in [AXIS1, AXIS2, SELECTED_ALGOS]:
        b.set_visible(PLOT_TYPE.get_value() == "correlation")
    update()
DATA_TYPE = ButtonCycle(fig.add_axes([0.1, 0.2, 0.3, 0.05]), ["NK", "SAT"], callback=update_visibility)
PLOT_TYPE = ButtonCycle(fig.add_axes([0.6, 0.2, 0.3, 0.05]), ["anytime", "correlation"], callback=update_visibility)
AXIS1 = ButtonCycle(fig.add_axes([0.76, 0.14, 0.09, 0.05]), ["fitness", "neighbors", "jump"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=update)
AXIS2 = ButtonCycle(fig.add_axes([0.76, 0.08, 0.09, 0.05]), ["jump", "fitness", "neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=update)
SELECTED_ALGOS = ButtonCheck(fig.add_axes([0.6, 0.02, 0.15, 0.17]), ALGO_KEYS, update)
# endregion =====================================================================================

update_visibility()
plt.show()