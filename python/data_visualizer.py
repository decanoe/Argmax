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
AXIS1_WHEN: ButtonCycle = None
AXIS2_WHEN: ButtonCycle = None
X_SCALE: ButtonCycle = None

ALGO_KEYS: list[str] = []

# ===============================================================================================

class NKRunInfo:
    data: pd.DataFrame
    name: str
    
    N: int
    K: int
    algo: str
    instance: int
    
    def __init__(self, n: int, k: int, algo: str, instance: str = "avg", name: str = "avg"):
        self.name = name
        self.algo = algo
        self.N = n
        self.K = k
        self.instance = instance
    @classmethod
    def from_file(cls, path: str):
        name = os.path.basename(path)
        with open(path) as f: data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        k = int(os.path.basename(dir).removeprefix("K"))
        dir = os.path.dirname(dir)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        result = cls(n, k, algo, instance, name)
        result.data = data
        return result
        
    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"
NK_file_infos: dict[int, dict[int, dict[str, dict[int, NKRunInfo]]]] = {}

from tqdm import tqdm
dirs = list(os.walk(dir_path+"\\data\\local_search\\NK"))
for i in tqdm(range(len(dirs))):
    for f in dirs[i][2]:
        info: NKRunInfo = NKRunInfo.from_file(dirs[i][0] + "\\" + f)
        NK_file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, {})[info.instance] = info

N_keys = sorted(NK_file_infos.keys())
K_keys = sorted(NK_file_infos[N_keys[0]].keys())
ALGO_KEYS = sorted(NK_file_infos[N_keys[0]][K_keys[0]].keys())
ALGO_COLORS = { ALGO_KEYS[i]: plt.cm.rainbow(np.linspace(0, 1, len(ALGO_KEYS)))[i] for i in range(len(ALGO_KEYS)) }

def NK_plot_anytime_avg(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    if (X_SCALE.get_value() == "log scale"):
        ax.set_xscale('log')
        
    all_points_per_algo: dict[str, dict[int, int]] = {}
    for algo in ALGO_KEYS:
        all_points: dict[int, int] = {}
        
        for i, runinfo in NK_file_infos[N.get_value()][K.get_value()][algo].items():
            data = runinfo.data
            
            temp = data
            while len(temp) != 0:
                all_points.setdefault(temp.budget.iloc[0], 0)
                temp = data[data.fitness_after_jump > temp.fitness_after_jump.iloc[0]]
            
            all_points.setdefault(data.budget.max(), 0)
        
        for x in all_points.keys():
            for i, runinfo in NK_file_infos[N.get_value()][K.get_value()][algo].items():
                data = runinfo.data
                all_points[x] += data[data.budget <= x].fitness_after_jump.max()
            all_points[x] /= len(NK_file_infos[N.get_value()][K.get_value()][algo])
        
        all_points_per_algo[algo] = all_points
    
    sorted_algos = sorted(ALGO_KEYS, key=lambda algo:max(all_points_per_algo[algo].values()), reverse=True)
    for algo in sorted_algos:
        x, y = zip(*sorted(all_points_per_algo[algo].items()))
        line, = ax.plot(x, y, label=algo, color = ALGO_COLORS[algo])
        legends.append(algo)
        lines.append(line)
        
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    ax.grid()
    return lines, legends
def NK_plot_anytime(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    if (X_SCALE.get_value() == "log scale"):
        ax.set_xscale('log')
    
    sorted_algos = sorted(ALGO_KEYS, key=lambda algo:NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()].data.fitness_after_jump.max(), reverse=True)
    
    for algo in sorted_algos:
        data = NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()].data
        
        all_x = []
        all_y = []
        
        current_x = 0
        temp = data
        while len(temp) != 0:
            current_x = temp.budget.iloc[0]
            all_x.append(current_x)
            all_y.append(temp.fitness_after_jump.iloc[0])
            temp = data[data.fitness_after_jump > all_y[-1]]
        
        max_x = data.budget.max()
        if (current_x != max_x):
            all_x.append(max_x)
            all_y.append(all_y[-1])
        
        line, = ax.plot(all_x, all_y, label=algo, color = ALGO_COLORS[algo])
        legends.append(algo)
        lines.append(line)
        
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    ax.grid()
    return lines, legends
def NK_plot_correlation(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    legends = []
    
    axis1: str = AXIS1.get_value()
    if (axis1 != "size_of_the_jump"):
        axis1 += AXIS1_WHEN.get_value()
    axis2: str = AXIS2.get_value()
    if (axis2 != "size_of_the_jump"):
        axis2 += AXIS2_WHEN.get_value()
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        algo = ALGO_KEYS[i]
        if not(SELECTED_ALGOS.button.get_status()[i]):
            continue
         
        infos: list[NKRunInfo] = []
        if (I.get_value() == "avg"):
            infos = NK_file_infos[N.get_value()][K.get_value()][algo].values()
        else:
            infos = [NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()]]
        
        all_x: list[float] = []
        all_y: list[float] = []
        for info in infos:
            all_x += list(info.data[info.data.size_of_the_jump != 0][axis1].to_numpy())
            all_y += list(info.data[info.data.size_of_the_jump != 0][axis2].to_numpy())
            max_val = max(max_val, info.data[info.data.size_of_the_jump != 0][axis1].max(), info.data[info.data.size_of_the_jump != 0][axis2].max())
        
        p, = ax.plot(all_x, all_y, 'o', markersize=3, label=algo, alpha = min(1, 150 / len(all_x)))
        legends.append(algo)
        #region aproximation
        color = p.get_color()
        theta = np.polyfit(all_x, all_y, deg=1)
        model = np.poly1d(theta)
        x = np.linspace(min(all_x), max(all_x), 100)
        ax.plot(x, model(x), "--", c=color)
        legends.append("_" + algo + "_approximation")
        #endregion
    
    if (not(axis1.startswith("fitness")) and not(axis2.startswith("fitness"))):
        ax.plot([0, max_val], [0, max_val], color = "lightgrey")
        legends.append("x=y")
    
    ax.set_xlabel(AXIS1.get_label())
    ax.set_ylabel(AXIS2.get_label())
    ax.grid()
    return [], legends
def NK_plot(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    if (PLOT_TYPE.get_value() == "anytime"):
        if (I.get_value() == "avg"):
            return NK_plot_anytime_avg(fig, ax)
        else:
            return NK_plot_anytime(fig, ax)
    return NK_plot_correlation(fig, ax)

# region output_table
def add_spaces(value: float, str_size: int):
    return str(value) + " " * (str_size - len(str(value)))
def NK_generate_table():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(algo, CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    budgets: list[int] = [100000, 50000, 10000, 5000, 1000]
    best_budget_content: list[str] = [avg_content for _ in budgets]
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in n_data.items():
            # region avg
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            values = []
            for algo, algo_data in k_data.items():
                values.append(0)
                count = 0
                for i_data in algo_data.values():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1)]
                    values[-1] += float(ends.fitness_after_jump.sum())
                    count += len(ends)
                values[-1] /= count
            
            best = max(values)
            for v in values:
                if (v == best):
                    avg_content += " | " + add_spaces(f"<b>{round(v, 6)}</b>", CELL_SIZE)
                else:
                    avg_content += " | " + add_spaces(round(v, 6), CELL_SIZE)
            # endregion
            
            # region best_budget_content
            for budget_index in range(len(budgets)):
                best_budget_content[budget_index] += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
                values = []
                for algo, algo_data in k_data.items():
                    values.append(0)
                    for i_data in algo_data.values():
                        values[-1] += i_data.data[i_data.data.budget <= budgets[budget_index]].fitness_after_jump.max()
                    values[-1] /= len(algo_data.values())
                
                best = max(values)
                for v in values:
                    if (v == best):
                        best_budget_content[budget_index] += " | " + add_spaces(f"<b>{round(v, 6)}</b>", CELL_SIZE)
                    else:
                        best_budget_content[budget_index] += " | " + add_spaces(round(v, 6), CELL_SIZE)
            # endregion
    
    with open(dir_path + "/output/avg.txt", "w") as f: f.write(avg_content + " |")
    for budget_index in range(len(budgets)):
        with open(dir_path + f"/output/best_{budgets[budget_index]//1000}_000.txt", "w") as f: f.write(best_budget_content[budget_index] + " |")
def NK_avg_budget_per_algo():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(algo, CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * (CELL_SIZE-1) + ":"] * (len(ALGO_KEYS) + 1))
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in n_data.items():
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            for algo_data in k_data.values():
                value: float = 0
                count: int = 0
                for i_data in algo_data.values():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                    count += len(ends)
                    value += ends.in_run_budget.sum()
                avg_content += " | " + add_spaces(round(value / count, 1), CELL_SIZE)
    
    with open(dir_path + "/output/budgets.txt", "w") as f: f.write(avg_content + " |")

# NK_avg_budget_per_algo()
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
        leg = ax.legend(legends)
        for lh in leg.legend_handles:
            lh.set_alpha(1)
    
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
    I = ButtonCycle(fig.add_axes([0.32, 0.14, 0.1, 0.05]), ["I" + str(i) for i in I_keys] + ["AVG"], I_keys + ["avg"], update)
# endregion =====================================================================================

# region permanent buttons ======================================================================
def update_visibility():
    for b in [AXIS1, AXIS2, SELECTED_ALGOS]:
        b.set_visible(PLOT_TYPE.get_value() == "correlation")
    
    AXIS1_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS1.get_label() != "jump")
    AXIS2_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS2.get_label() != "jump")
    
    update()
def update_visibility_data_type():
    if (DATA_TYPE.get_value() == "NK"):
        update_NK()
    
    for b in [K]:
        b.set_visible(DATA_TYPE.get_value() == "NK")
    update_visibility()
DATA_TYPE = ButtonCycle(fig.add_axes([0.1, 0.2, 0.3, 0.05]), ["NK", "SAT"], callback=update_visibility_data_type)
PLOT_TYPE = ButtonCycle(fig.add_axes([0.6, 0.2, 0.3, 0.05]), ["anytime", "correlation"], callback=update_visibility)

X_SCALE = ButtonCycle(fig.add_axes([0.6, 0.14, 0.15, 0.05]), ["log scale", "linear scale"], callback=update)

AXIS1 = ButtonCycle(fig.add_axes([0.76, 0.14, 0.09, 0.05]), ["jump", "fitness", "improving neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=update_visibility)
AXIS2 = ButtonCycle(fig.add_axes([0.76, 0.08, 0.09, 0.05]), ["fitness", "improving neighbors", "jump"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=update_visibility)
AXIS1_WHEN = ButtonCycle(fig.add_axes([0.86, 0.14, 0.09, 0.05]), ["after jump", "before jump"], ["_after_jump", "_before_jump"], callback=update)
AXIS2_WHEN = ButtonCycle(fig.add_axes([0.86, 0.08, 0.09, 0.05]), ["after jump", "before jump"], ["_after_jump", "_before_jump"], callback=update)
SELECTED_ALGOS = ButtonCheck(fig.add_axes([0.6, 0.02, 0.15, 0.17]), ALGO_KEYS, update)
# endregion =====================================================================================

update_visibility_data_type()
plt.show()