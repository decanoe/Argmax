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
plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

FULL_SCREEN: bool = False
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
CORRELATION_PLOT: ButtonCycle = None
REGRESSION: ButtonCycle = None
X_SCALE: ButtonCycle = None
LEGEND_POSITION: ButtonCycle = None

ALGO_KEYS: list[str] = []
LABEL_TRANSLATIONS: dict[str, str] = {}
ALGO_COLORS = {}
ALGO_LINESTYLE = {}

# region greedy_all
LABEL_TRANSLATIONS["greedy_all_best"] = "GJ_full_best"
LABEL_TRANSLATIONS["greedy_all_first"] = "GJ_full_first"
LABEL_TRANSLATIONS["greedy_all_least"] = "GJ_full_least"

ALGO_COLORS["greedy_all_best"] = "blue"
ALGO_COLORS["greedy_all_first"] = "green"
ALGO_COLORS["greedy_all_least"] = "red"

ALGO_LINESTYLE["greedy_all_best"] = "solid"
ALGO_LINESTYLE["greedy_all_first"] = "solid"
ALGO_LINESTYLE["greedy_all_least"] = "solid"
# endregion

# region greedy_improve
LABEL_TRANSLATIONS["greedy_improve_best"] = "GJ_improve_best"
LABEL_TRANSLATIONS["greedy_improve_first"] = "GJ_improve_first"
LABEL_TRANSLATIONS["greedy_improve_least"] = "GJ_improve_least"

ALGO_COLORS["greedy_improve_best"] = "cyan"
ALGO_COLORS["greedy_improve_first"] = "lime"
ALGO_COLORS["greedy_improve_least"] = "orange"

ALGO_LINESTYLE["greedy_improve_best"] = "solid"
ALGO_LINESTYLE["greedy_improve_first"] = "solid"
ALGO_LINESTYLE["greedy_improve_least"] = "solid"
# endregion

# region hill_climb
LABEL_TRANSLATIONS["hc_first"] = None
LABEL_TRANSLATIONS["hc_cycle"] = None
LABEL_TRANSLATIONS["hc_best"] = "HC_best"
LABEL_TRANSLATIONS["hc_random"] = "HC_first"
LABEL_TRANSLATIONS["hc_least"] = "HC_least"

ALGO_COLORS["hc_best"] = ALGO_COLORS["greedy_all_best"]
ALGO_COLORS["hc_random"] = ALGO_COLORS["greedy_all_first"]
ALGO_COLORS["hc_least"] = ALGO_COLORS["greedy_all_least"]

ALGO_LINESTYLE["hc_best"] = "dashed"
ALGO_LINESTYLE["hc_random"] = "dashed"
ALGO_LINESTYLE["hc_least"] = "dashed"
# endregion

# region greedy_fixed
LABEL_TRANSLATIONS["greedy_fixed_best_.5"] = "GJ_1/2_best"
LABEL_TRANSLATIONS["greedy_fixed_best_.25"] = "GJ_1/4_best"
LABEL_TRANSLATIONS["greedy_fixed_best_.75"] = "GJ_3/4_best"

LABEL_TRANSLATIONS["greedy_fixed_first_.5"] = "GJ_1/2_first"
LABEL_TRANSLATIONS["greedy_fixed_first_.25"] = "GJ_1/4_first"
LABEL_TRANSLATIONS["greedy_fixed_first_.75"] = "GJ_3/4_first"

LABEL_TRANSLATIONS["greedy_fixed_least_.5"] = "GJ_1/2_least"
LABEL_TRANSLATIONS["greedy_fixed_least_.25"] = "GJ_1/4_least"
LABEL_TRANSLATIONS["greedy_fixed_least_.75"] = "GJ_3/4_least"


ALGO_COLORS["greedy_fixed_best_.5"] = ALGO_COLORS["greedy_all_best"]
ALGO_COLORS["greedy_fixed_best_.25"] = ALGO_COLORS["greedy_all_best"]
ALGO_COLORS["greedy_fixed_best_.75"] = ALGO_COLORS["greedy_all_best"]

ALGO_COLORS["greedy_fixed_first_.5"] = ALGO_COLORS["greedy_all_first"]
ALGO_COLORS["greedy_fixed_first_.25"] = ALGO_COLORS["greedy_all_first"]
ALGO_COLORS["greedy_fixed_first_.75"] = ALGO_COLORS["greedy_all_first"]

ALGO_COLORS["greedy_fixed_least_.5"] = ALGO_COLORS["greedy_all_least"]
ALGO_COLORS["greedy_fixed_least_.25"] = ALGO_COLORS["greedy_all_least"]
ALGO_COLORS["greedy_fixed_least_.75"] = ALGO_COLORS["greedy_all_least"]


ALGO_LINESTYLE["greedy_fixed_best_.5"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_best_.25"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_best_.75"] = "dashdot"

ALGO_LINESTYLE["greedy_fixed_first_.5"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_first_.25"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_first_.75"] = "dashdot"

ALGO_LINESTYLE["greedy_fixed_least_.5"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_least_.25"] = "dashdot"
ALGO_LINESTYLE["greedy_fixed_least_.75"] = "dashdot"
# endregion

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
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        k = int(os.path.basename(dir).removeprefix("K"))
        dir = os.path.dirname(dir)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        if (LABEL_TRANSLATIONS[algo] == None):
            return None
        
        with open(path) as f: data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
        data.fitness_before_jump /= n
        data.fitness_after_jump /= n
        
        result = cls(n, k, algo, instance, name)
        result.data = data
        return result
        
    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"
NK_file_infos: dict[int, dict[int, dict[str, dict[int, NKRunInfo]]]] = {}

from tqdm import tqdm
dirs = list(os.walk(dir_path+"/data/local_search/NK"))
for i in tqdm(range(len(dirs))):
    for f in dirs[i][2]:
        info: NKRunInfo = NKRunInfo.from_file(dirs[i][0] + "/" + f)
        if (info != None):
            NK_file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, {})[info.instance] = info

N_keys = sorted(NK_file_infos.keys())
K_keys = sorted(NK_file_infos[N_keys[0]].keys())
ALGO_KEYS = sorted(NK_file_infos[N_keys[0]][K_keys[0]].keys())

# for each (N, K), for each algo, 2 matrices with the first being the budget and the second being the average maximum reached
NK_AVG_POINTS: dict[tuple[int, int], dict[str, tuple[np.ndarray[int], np.ndarray[float]]]] = {}
def construct_NK_avg_points(n: int, k: int) -> dict[str, dict[int, int]]:
    global NK_AVG_POINTS
    if ((n, k) in NK_AVG_POINTS):
        return NK_AVG_POINTS.get((n, k))
    
    all_points_per_algo: dict[str, (np.ndarray[int], np.ndarray[float])] = {}
    for algo in ALGO_KEYS:
        budget_points: np.ndarray[int] = np.array([])
        fitness_points: np.ndarray[float] = np.array([])
        
        for runinfo in NK_file_infos[n][k][algo].values():
            budget_points = np.unique(np.concatenate((budget_points, runinfo.data.budget)))
            budget_points.sort(kind='mergesort')
        
        for runinfo in NK_file_infos[n][k][algo].values():
            temp: np.ndarray[float] = np.array([runinfo.data.budget, runinfo.data.fitness_after_jump])
            anytime_fitness: np.ndarray[float] = np.maximum.accumulate(temp, axis=1)
            
            idx: np.ndarray[int] = np.searchsorted(anytime_fitness[0,:], budget_points, side="right") - 1
            mask: np.ndarray[bool] = idx < 0
            idx: np.ndarray[int] = np.clip(idx, 0, len(anytime_fitness[0,:]) - 1)
            values: np.ndarray[float] = anytime_fitness[1,idx]
            values[mask] = 0
            
            if (fitness_points.shape[0] == 0):
                fitness_points = values
            else:
                fitness_points = np.vstack((fitness_points, values))
        
        all_points_per_algo[algo] = (budget_points, fitness_points.mean(axis=0))
    
    NK_AVG_POINTS[(n, k)] = all_points_per_algo
    return all_points_per_algo
def NK_plot_anytime_avg(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    if (X_SCALE.get_value() == "log scale"):
        ax.set_xscale('log')
        
    all_points_per_algo: dict[str, tuple[np.ndarray[int], np.ndarray[float]]] = construct_NK_avg_points(N.get_value(), K.get_value())

    sorted_algos = sorted(ALGO_KEYS, key=lambda algo: all_points_per_algo[algo][1][-1], reverse=True)
    for algo in sorted_algos:
        x, y = all_points_per_algo[algo]
        line, = ax.plot(x, y, label=LABEL_TRANSLATIONS[algo], color = ALGO_COLORS[algo], linestyle = ALGO_LINESTYLE[algo])
        legends.append(LABEL_TRANSLATIONS[algo])
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
        
        line, = ax.plot(all_x, all_y, label=LABEL_TRANSLATIONS[algo], color = ALGO_COLORS[algo], linestyle = ALGO_LINESTYLE[algo])
        legends.append(LABEL_TRANSLATIONS[algo])
        lines.append(line)
        
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    ax.grid()
    return lines, legends
def _NK_get_single_correlation_points(algo: str, axis1: str, axis2: str) -> tuple[np.ndarray[float], np.ndarray[float]]:
    infos: list[NKRunInfo] = []
    if (I.get_value() == "avg"):
        infos = NK_file_infos[N.get_value()][K.get_value()][algo].values()
    else:
        infos = [NK_file_infos[N.get_value()][K.get_value()][algo][I.get_value()]]
        
    all_x: np.ndarray[float] = np.array([])
    all_y: np.ndarray[float] = np.array([])
    for info in infos:
        data: pd.DataFrame = info.data[info.data.size_of_the_jump != 0]

        temp_x: np.ndarray[float] = data[axis1.replace("_delta", "_after_jump")].to_numpy(copy=True)
        if (axis1.endswith("_delta")):
            temp_x -= data[axis1.replace("_delta", "_before_jump")].to_numpy()
        
        temp_y: np.ndarray[float] = data[axis2.replace("_delta", "_after_jump")].to_numpy(copy=True)
        if (axis2.endswith("_delta")):
            temp_y -= data[axis2.replace("_delta", "_before_jump")].to_numpy()
        
        all_x = np.concatenate((all_x, temp_x), axis=None)
        all_y = np.concatenate((all_y, temp_y), axis=None)
    
    return all_x, all_y
def _NK_plot_correlation_regression(fig: plt.Figure, ax: plt.Axes, axis1: str, axis2: str) -> tuple[list[plt.Line2D], list[str], float]:
    lines = []
    legends = []
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        algo = ALGO_KEYS[i]
        if not(SELECTED_ALGOS.button.get_status()[i]):
            continue
        
        all_x, all_y = _NK_get_single_correlation_points(algo, axis1, axis2)
        max_val = max(max_val, all_x.max(), all_y.max())
        
        p, = ax.plot(all_x, all_y, linestyle="", marker='o', color = ALGO_COLORS[algo], markersize=3, label=LABEL_TRANSLATIONS[algo], alpha = min(1, 150 / len(all_x)))
        legends.append("")
        lines.append(p)
        
        #region aproximation
        theta = np.polyfit(all_x, all_y, deg=REGRESSION.get_value())
        model = np.poly1d(theta)
        x = np.linspace(all_x.min(), all_x.max(), 100)
        line, = ax.plot(x, model(x), color = ALGO_COLORS[algo], linestyle = ALGO_LINESTYLE[algo], label = LABEL_TRANSLATIONS[algo] + " regression")
        legends.append(LABEL_TRANSLATIONS[algo] + " regression")
        lines.append(line)
        #endregion
    
    return lines, legends, max_val
def _NK_plot_correlation_mean_std(fig: plt.Figure, ax: plt.Axes, axis1: str, axis2: str) -> tuple[list[plt.Line2D], list[str], float]:
    lines = []
    legends = []
    
    if (axis1.startswith("fitness") and axis2.startswith("fitness")):
        return lines, legends, 1
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        algo = ALGO_KEYS[i]
        if not(SELECTED_ALGOS.button.get_status()[i]):
            continue
        
        all_x, all_y = _NK_get_single_correlation_points(algo, axis1, axis2)
        max_val = max(max_val, all_x.max(), all_y.max())
        
        discrete_values: np.ndarray[float] = np.unique(all_y if axis1.startswith("fitness") else all_x)
        mean: np.ndarray[float] = np.zeros(discrete_values.size)
        std: np.ndarray[float] = np.zeros(discrete_values.size)
        
        for i in range(discrete_values.size):
            if axis1.startswith("fitness"):
                mask = (all_y == discrete_values[i])
                mean[i] = all_x[mask].mean()
                std[i] = all_x[mask].std()
            else:
                mask = (all_x == discrete_values[i])
                mean[i] = all_y[mask].mean()
                std[i] = all_y[mask].std()
        
        line1, line2 = (None, None)
        if (axis1.startswith("fitness")):
            ax.fill_betweenx(discrete_values, mean - std, mean + std, alpha=.5, linewidth=0, color = ALGO_COLORS[algo])
            line2, = ax.plot(mean, discrete_values, linewidth=2, color = ALGO_COLORS[algo], linestyle = ALGO_LINESTYLE[algo], label=LABEL_TRANSLATIONS[algo] + " mean")
        else:
            ax.fill_between(discrete_values, mean - std, mean + std, alpha=.5, linewidth=0, color = ALGO_COLORS[algo])
            line2, = ax.plot(discrete_values, mean, linewidth=2, color = ALGO_COLORS[algo], linestyle = ALGO_LINESTYLE[algo], label=LABEL_TRANSLATIONS[algo] + " mean")
        
        legends.append("")
        lines.append(line1)
        legends.append(LABEL_TRANSLATIONS[algo] + " mean")
        lines.append(line2)
    
    return lines, legends, max_val
def NK_plot_correlation(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    axis1: str = AXIS1.get_value()
    if (axis1 != "size_of_the_jump"):
        axis1 += AXIS1_WHEN.get_value()
    axis2: str = AXIS2.get_value()
    if (axis2 != "size_of_the_jump"):
        axis2 += AXIS2_WHEN.get_value()
    
    lines = []
    legends = []
    max_val = 1
    if (CORRELATION_PLOT.get_value() == "regression"):
        lines, legends, max_val = _NK_plot_correlation_regression(fig, ax, axis1, axis2)
    else:
        lines, legends, max_val = _NK_plot_correlation_mean_std(fig, ax, axis1, axis2)

    if (not(AXIS1.get_value() == "fitness") and not(AXIS2.get_value() == "fitness") and not(axis1.endswith("delta")) and not(axis2.endswith("delta"))):
        line, = ax.plot(np.linspace(0, max_val), np.linspace(0, max_val), color = "gray", linestyle = "--", label = "x = y")
        legends.append("x=y")
        lines.append(line)
    ax.set_xlabel(AXIS1.get_label() + (" " + AXIS1_WHEN.get_label() if AXIS1.get_label() != "jump size" else ""))
    ax.set_ylabel(AXIS2.get_label() + (" " + AXIS2_WHEN.get_label() if AXIS2.get_label() != "jump size" else ""))
    ax.grid()
    return lines, legends
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

COUNT_PER_INSTANCE: int = 2 # nb of runs to take per instance (should not exceed the number of runs done on each instances)

def NK_generate_avg_table_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(LABEL_TRANSLATIONS.get(algo, algo), CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            values = []
            for algo in ALGO_KEYS:
                algo_data = k_data[algo]
                values.append(0)
                count = 0
                for i_data in algo_data.values():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                    nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                    if (nb_ends < COUNT_PER_INSTANCE):
                        print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                    
                    count += nb_ends
                    values[-1] += ends.fitness_after_jump[:nb_ends].sum()
                values[-1] /= count
                values[-1] = round(values[-1], 4)
            
            for value in values:
                if value == max(values):
                    avg_content += " | " + add_spaces(f"<ins>{value}</ins>", CELL_SIZE)
                else:
                    avg_content += " | " + add_spaces(value, CELL_SIZE)
            
    with open(dir_path + "/output/avg.txt", "w") as f: f.write(avg_content + " |")
def NK_generate_avg_table_latex():
    CELL_SIZE = 7
    
    header: str = """\\begin{table}[ht]
\\caption{Mean fitness score per execution. Results are averaged over """ + str(COUNT_PER_INSTANCE * 10) + """ instances per $(N,K)$ pair. The best results for each instance are underlined.}
\\label{tab:mean_scores}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
    footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""
    
    body: str = ""
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            body += "\\textbf{" + str(n) + ", " + str(k) + "} "
            line_values = []
            
            for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                for algo_param in ["best", "first", "least"]:
                    algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                    algo_data = k_data[algo]
                    value: float = 0
                    count: int = 0
                    for i, i_data in algo_data.items():
                        ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                        nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                        if (nb_ends < COUNT_PER_INSTANCE):
                            print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                        
                        count += nb_ends
                        value += ends.fitness_after_jump[:nb_ends].sum()
                    line_values.append(round(value / count, 4))
            
            for value in line_values:
                if value == max(line_values):
                    body += "& " + add_spaces("\\underline{" + "%01.4f"%value + "}", CELL_SIZE) + " "
                else:
                    body += "& " + add_spaces("%01.4f"%value, CELL_SIZE) + " "
            body += " \\\\\n"
        body += "\\hline\n"
    
    with open(dir_path + "/output/avg.tex", "w") as f: f.write(header + body + footer)

def NK_generate_budget_tables_md():
    CELL_SIZE = 16
    
    header: str = "| " + " | ".join([add_spaces(LABEL_TRANSLATIONS.get(algo, algo), CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    header += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    budgets: list[int] = [100_000, 10_000, 1_000]
    best_budget_content: list[str] = [header for _ in budgets]
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            for budget_index in range(len(budgets)):
                best_budget_content[budget_index] += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
                values = []
                for algo in ALGO_KEYS:
                    algo_data = k_data[algo]
                    values.append(0)
                    for i_data in algo_data.values():
                        values[-1] += i_data.data[i_data.data.budget <= budgets[budget_index]].fitness_after_jump.max()
                    values[-1] /= len(algo_data.values())
                    values[-1] = round(values[-1], 4)
                
                for value in values:
                    if value == max(values):
                        best_budget_content[budget_index] += " | " + add_spaces(f"<ins>{"%01.4f"%value}</ins>", CELL_SIZE)
                    else:
                        best_budget_content[budget_index] += " | " + add_spaces("%01.4f"%value, CELL_SIZE)
    
    for budget_index in range(len(budgets)):
        with open(dir_path + f"/output/best_{budgets[budget_index]//1000}_000.txt", "w") as f: f.write(best_budget_content[budget_index] + " |")
def NK_generate_budget_tables_latex():
    CELL_SIZE = 9
    
    budgets: list[int] = [1_000, 10_000, 100_000]
    budgets_text = [
        "Best fitness achieved under a limited budget of 1,000 evaluations. Underlined values indicate the best performance for each instance.",
        "Best score obtained for a budget of 10,000 evaluations.",
        "Best fitness achieved under a large budget of 100,000 evaluations. Underlined values indicate the best performance for each instance."
    ]
    budgets_label = ["tab:budget1k", "tab:budget10k", "tab:budget100k"]
    
    for budget, budget_text, budget_label in zip(budgets, budgets_text, budgets_label):
        header: str = """\\begin{table}[h]
\\caption{""" + budget_text + """}
\\label{""" + budget_label + """}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
# \\textbf{50, 0} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} \\\\
# \\textbf{50, 1} & 0.6988 & 0.6980 & \\underline{0.6995} & 0.6994 & 0.6989 & 0.6954 & 0.6972 & 0.6979 & 0.6867 \\\\
# \\textbf{50, 2} & 0.7422 & \\underline{0.7442} & 0.7424 & 0.7402 & 0.7441 & 0.7349 & 0.7358 & 0.7390 & 0.7210 \\\\
# \\textbf{50, 4} & 0.7558 & \\underline{0.7621} & 0.7596 & 0.7538 & 0.7618 & 0.7611 & 0.7456 & 0.7536 & 0.7212 \\\\
# \\textbf{50, 8} & 0.7532 & 0.7464 & 0.7347 & 0.7523 & 0.7500 & 0.7476 & 0.7449 & \\underline{0.7560} & 0.7026 \\\\
# \\hline
# \\textbf{100, 0} & \\underline{0.6643} & 0.6558 & 0.6268 & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} \\\\
# \\textbf{100, 1} & 0.6971 & 0.6866 & 0.6515 & 0.6971 & \\underline{0.6972} & 0.6877 & 0.6908 & 0.6929 & 0.6763 \\\\
# \\textbf{100, 2} & 0.7379 & 0.7272 & 0.6947 & \\underline{0.7398} & 0.7394 & 0.7308 & 0.7297 & 0.7371 & 0.7109 \\\\
# \\textbf{100, 4} & 0.7458 & 0.7642 & 0.7014 & 0.7492 & \\underline{0.7656} & 0.7574 & 0.7368 & 0.7528 & 0.5899 \\\\
# \\textbf{100, 8} & 0.7348 & 0.7346 & 0.6677 & 0.7328 & 0.7405 & \\underline{0.7441} & 0.7207 & 0.7413 & 0.5444 \\\\
# \\hline
        footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""

        body: str = ""
    
        for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
            for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
                body += "\\textbf{" + str(n) + ", " + str(k) + "} "
                line_values = []
                
                for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                    for algo_param in ["best", "first", "least"]:
                        algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                        algo_data = k_data[algo]
                        
                        line_values.append(0)
                        for i_data in algo_data.values():
                            line_values[-1] += i_data.data[i_data.data.budget <= budget].fitness_after_jump.max()
                        line_values[-1] /= len(algo_data.values())
                        line_values[-1] = round(line_values[-1], 4)
                
                for value in line_values:
                    if value == max(line_values):
                        body += "& " + add_spaces("\\underline{" + "%01.4f"%value + "}", CELL_SIZE) + " "
                    else:
                        body += "& " + add_spaces("%01.4f"%value, CELL_SIZE) + " "
                body += " \\\\\n"
            body += "\\hline\n"
        
        with open(dir_path + f"/output/best_{budget//1000}_000.tex", "w") as f: f.write(header + body + footer)

def NK_avg_budget_per_algo_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(LABEL_TRANSLATIONS.get(algo, algo), CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            line_values = []
            
            for algo in ALGO_KEYS:
                algo_data = k_data[algo]
                value: float = 0
                count: int = 0
                for i, i_data in algo_data.items():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                    nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                    if (nb_ends < COUNT_PER_INSTANCE):
                        print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                    
                    count += nb_ends
                    value += float(ends.in_run_budget[:nb_ends].sum())
                line_values.append(round(value / count, 1))
            
            for value in line_values:
                if value == min(line_values):
                    avg_content += " | " + add_spaces("<ins>" + "%.1f"%value + "</ins>", CELL_SIZE)
                else:
                    avg_content += " | " + add_spaces("%.1f"%value, CELL_SIZE)
    
    with open(dir_path + "/output/budgets.txt", "w") as f: f.write(avg_content + " |")
def NK_avg_budget_per_algo_latex():
    CELL_SIZE = 7
    
    header: str = """\\begin{table}[ht]
\\caption{Mean evaluation budget per single trajectory. Data averaged over """ + str(COUNT_PER_INSTANCE * 10) + """ independent runs per $(N,K)$ pair across 10 problem instances.}
\\label{tab:mean_budget}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
    footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""
    
    body: str = ""
    
    for n, n_data in sorted(NK_file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            body += "\\textbf{" + str(n) + ", " + str(k) + "} "
            line_values = []
            
            for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                for algo_param in ["best", "first", "least"]:
                    algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                    algo_data = k_data[algo]
                    value: float = 0
                    count: int = 0
                    for i, i_data in algo_data.items():
                        ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                        nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                        if (nb_ends < COUNT_PER_INSTANCE):
                            print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                        
                        count += nb_ends
                        value += float(ends.in_run_budget[:nb_ends].sum())
                    line_values.append(round(value / count, 1))
            
            for value in line_values:
                if value == min(line_values):
                    body += "& " + add_spaces("\\underline{" + "%.1f"%value + "}", CELL_SIZE) + " "
                else:
                    body += "& " + add_spaces("%.1f"%value, CELL_SIZE) + " "
            body += " \\\\\n"
        body += "\\hline\n"
    
    with open(dir_path + "/output/budgets.tex", "w") as f: f.write(header + body + footer)

# NK_avg_budget_per_algo_md()
# NK_avg_budget_per_algo_latex()

# NK_generate_avg_table_md()
# NK_generate_avg_table_latex()

# NK_generate_budget_tables_md()
# NK_generate_budget_tables_latex()
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
        leg = ax.legend(
            [lines[i] for i in range(len(legends)) if legends[i] != ""],
            [l for l in legends if l != ""],
            loc=LEGEND_POSITION.get_value()
            )
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
    if (FULL_SCREEN):
        if (vis):
            annot.set_visible(False)
            fig.canvas.draw_idle()
        return
    
    if event.inaxes == ax:
        found: bool = False
        
        for index in range(len(lines)):
            if (lines[index] == None):
                continue
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
    
    if (N == None):
        N = ButtonCycle(fig.add_axes([0.1, 0.14, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, update)
    else:
        N.set_labels(["N" + str(i) for i in N_keys])
        N.set_values(N_keys)
    
    if (K == None):
        K = ButtonCycle(fig.add_axes([0.21, 0.14, 0.1, 0.05]), ["K" + str(i) for i in K_keys], K_keys, update)
    else:
        K.set_labels(["K" + str(i) for i in K_keys])
        K.set_values(K_keys)
    
    if (I == None):
        I = ButtonCycle(fig.add_axes([0.32, 0.14, 0.1, 0.05]), ["I" + str(i) for i in I_keys] + ["AVG"], I_keys + ["avg"], update)
    else:
        I.set_labels(["I" + str(i) for i in I_keys] + ["AVG"])
        I.set_values(I_keys + ["avg"])
# endregion =====================================================================================

# region permanent buttons ======================================================================
def update_visibility():
    for b in [SELECTED_ALGOS, AXIS1, AXIS2, CORRELATION_PLOT]:
        b.set_visible(PLOT_TYPE.get_value() == "correlation" and not(FULL_SCREEN))
    for b in [X_SCALE]:
        b.set_visible(PLOT_TYPE.get_value() != "correlation" and not(FULL_SCREEN))
    
    AXIS1_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS1.get_value() != "size_of_the_jump" and not(FULL_SCREEN))
    AXIS2_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS2.get_value() != "size_of_the_jump" and not(FULL_SCREEN))
    
    REGRESSION.set_visible(PLOT_TYPE.get_value() == "correlation" and CORRELATION_PLOT.get_value() == "regression" and not(FULL_SCREEN))
    
    update()
def update_visibility_data_type():
    if (DATA_TYPE.get_value() == "NK"):
        update_NK()
    
    for b in [K]:
        b.set_visible(DATA_TYPE.get_value() == "NK" and not(FULL_SCREEN))
    update_visibility()
DATA_TYPE = ButtonCycle(fig.add_axes([0.1, 0.2, 0.3, 0.05]), ["NK", "SAT"], callback=update_visibility_data_type)
PLOT_TYPE = ButtonCycle(fig.add_axes([0.6, 0.2, 0.3, 0.05]), ["anytime", "correlation"], callback=update_visibility)

X_SCALE = ButtonCycle(fig.add_axes([0.6, 0.14, 0.15, 0.05]), ["log scale", "linear scale"], callback=update)
LEGEND_POSITION = ButtonCycle(fig.add_axes([0.1, 0.08, 0.1, 0.05]), ["best", "upper right", "upper left", "lower left", "lower right"], [i for i in range(5)], update)

SELECTED_ALGOS = ButtonCheck(fig.add_axes([0.6, 0.02, 0.15, 0.17]), ALGO_KEYS, update)

AXIS1 = ButtonCycle(fig.add_axes([0.76, 0.14, 0.09, 0.05]), ["jump size", "fitness", "improving neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=update_visibility)
AXIS2 = ButtonCycle(fig.add_axes([0.76, 0.08, 0.09, 0.05]), ["fitness", "improving neighbors", "jump size"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=update_visibility)
AXIS1_WHEN = ButtonCycle(fig.add_axes([0.86, 0.14, 0.09, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=update)
AXIS2_WHEN = ButtonCycle(fig.add_axes([0.86, 0.08, 0.09, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=update)

CORRELATION_PLOT = ButtonCycle(fig.add_axes([0.76, 0.02, 0.09, 0.05]), ["regression", "mean+std"], callback=update_visibility)
REGRESSION = ButtonCycle(fig.add_axes([0.86, 0.02, 0.09, 0.05]), ["linear", "quadratic", "degree 3"], [1, 2, 3], callback=update)

# endregion =====================================================================================

# region full_screen ============================================================================
def switch_full_screen(key: str):
    global FULL_SCREEN, fig
    
    if (key != "' '"):
        return
    
    FULL_SCREEN = not(FULL_SCREEN)
    if (FULL_SCREEN):
        fig.subplots_adjust(left=0.075, right=0.98, bottom=0.1, top=0.95)
        plt.rcParams.update({'font.size': 22})
    else:
        fig.subplots_adjust(left=0.1, right=0.9, bottom=0.35, top=0.95)
        plt.rcParams.update({'font.size': 10})
    
    for b in [DATA_TYPE, PLOT_TYPE, X_SCALE, LEGEND_POSITION, AXIS1, AXIS2, AXIS1_WHEN, AXIS2_WHEN, REGRESSION, SELECTED_ALGOS, N, K, I]:
        b.set_visible(not(FULL_SCREEN))
    update_visibility_data_type()
    
fig.canvas.mpl_connect('key_press_event', lambda evt: switch_full_screen(repr(evt.key)))
# fig.canvas.mpl_connect('key_press_event', lambda evt: print(repr(evt.key)))
# endregion =====================================================================================

update_visibility_data_type()
SELECTED_ALGOS.check_all([algo for algo in ALGO_KEYS if LABEL_TRANSLATIONS.get(algo, algo).startswith("GJ")], True)
plt.show()