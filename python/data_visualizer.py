import os
import sys
import pandas as pd
import numpy as np
import io
import re

dir_path = os.path.dirname(os.path.realpath(__file__))

if __name__ != "__main__":
    exit()

# ===============================================================================================

import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from matplotlib.backend_bases import MouseEvent
from matplotlib.text import Annotation
from button import ButtonCycle, ButtonCheck, KeyHoldEvent
plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

FULL_SCREEN: bool = False
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

def get_label_and_style(algo: str)->dict[str, str]:
    full_algo = algo
    def translate_amount(amount: str):
        try:
            v = float(amount)
            return f"{int(v * 100)}%"
        except ValueError:
            print(f"can't translate amount for {amount} !")
            exit(0)
    reference_color = {
        "best": "blue",
        "first": "orange",
        "least": "red",
        "random": "green",
    }
    
    if (algo.startswith("greedy_tabu")):
        algo = algo.removeprefix("greedy_tabu").removeprefix("_")
        push_translations = {
            "BestToWorst": "BtW",
            "BestToWorstClamped": "BtWc",
            "WorstToBest": "WtB",
        }
        
        for push, push_tr in push_translations.items():
            if (algo.startswith(push+"_")):
                algo = algo.removeprefix(push).removeprefix("_")
                
                tabu_size: str = algo.split("_")[0]
                tabu_random: str = algo.split("_")[1].removeprefix("r")
                return {
                    "label": f"GJ tabu {push_tr} {translate_amount(tabu_size)}~{translate_amount(tabu_random)}",
                    "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
                    "color": "cyan",
                }
        print(f"malformed algo file label : {full_algo} !")
        exit(0)
    
    if (algo.startswith("tabu")):
        algo = algo.removeprefix("tabu").removeprefix("_")
                
        tabu_size: str = algo.split("_")[0]
        tabu_random: str = algo.split("_")[1].removeprefix("r")
        return {
            "label": f"tabu {translate_amount(tabu_size)}~{translate_amount(tabu_random)}",
            "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
            "color": "gray",
        }
    
    if (algo.startswith("greedy")):
        algo = algo.removeprefix("greedy").removeprefix("_")
        greedy_scope_translations = {
            "all": "full",
            "improve": "improve",
            "fixed": "top",
        }
        greedy_criterion_translations = {
            "best": "best",
            "first": "maximal",
            "least": "least",
            "random": "first",
        }
        
        for scope, scope_tr in greedy_scope_translations.items():
            if (algo.startswith(scope)):
                algo = algo.removeprefix(scope).removeprefix("_")
                
                for criterion, criterion_tr in greedy_criterion_translations.items():
                    if (algo.startswith(criterion)):
                        algo = algo.removeprefix(criterion).removeprefix("_")
                        
                        if (scope == "fixed"):
                            return {
                                    "label": f"GJ {scope_tr}-{translate_amount(algo)} {criterion_tr}",
                                    "linestyle": (5, (10, 3)), # long dash with offset
                                    "color": reference_color[criterion],
                                }
                        else:
                            return {
                                    "label": f"GJ {scope_tr} {criterion_tr}",
                                    "linestyle": "solid" if scope == "all" else "dashdot",
                                    "color": reference_color[criterion],
                                }

        print(f"malformed algo file label : {full_algo} !")
        exit(0)
    
    if (algo.startswith("hc")):
        algo = algo.removeprefix("hc").removeprefix("_")
        hc_criterion_translations = {
            "best": "best",
            "least": "least",
            "random": "first",
            "first": "first(index based)",
            "cycle": "cycle",
        }
    
        for criterion, criterion_tr in hc_criterion_translations.items():
            if (algo.startswith(criterion)):
                algo = algo.removeprefix(criterion).removeprefix("_")
                return {
                        "label": f"HC {criterion_tr}",
                        "linestyle": "dashed",
                        "color": reference_color.get(criterion, "grey"),
                    }
                
        print(f"malformed algo file label : {full_algo} !")
        exit(0)
    
    print(f"algo file label not recognized : {full_algo} !")
    exit(0)
# ===============================================================================================

class NKRunInfo:
    data: pd.DataFrame
    name: str
    
    N: int
    K: int
    algo: str
    instance: int
    
    label: str
    linestyle: str
    color: str
    
    def __init__(self, n: int, k: int, algo: str, instance: str = "avg", name: str = "avg"):
        self.name = name
        self.algo = algo
        self.N = n
        self.K = k
        self.instance = instance
        
        label_and_style = get_label_and_style(algo)
        self.label = label_and_style["label"]
        self.linestyle = label_and_style["linestyle"]
        self.color = label_and_style["color"]
    @classmethod
    def from_file(cls, path: str):
        name = os.path.basename(path)
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        k = int(os.path.basename(dir).removeprefix("K"))
        dir = os.path.dirname(dir)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        with open(path) as f: data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
        data.fitness_before_jump /= n
        data.fitness_after_jump /= n
        
        result = cls(n, k, algo, instance, name)
        result.data = data
        return result
        
    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"
file_infos: dict[int, dict[int, dict[str, dict[int, NKRunInfo]]]] = {}

from tqdm import tqdm
files: list[tuple[str, str]] = []
for d in os.walk(dir_path+"/data/local_search/NK"):
    files += [(d[0], f) for f in d[2]]

for i in tqdm(range(len(files))):
    d, f = files[i]
    info: NKRunInfo = NKRunInfo.from_file(d + "/" + f)
    if (info != None):
        file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, {})[info.instance] = info

N_keys = sorted(file_infos.keys())
K_keys = sorted(file_infos[N_keys[0]].keys())
ALGO_KEYS = sorted(file_infos[N_keys[0]][K_keys[0]].keys())

def is_algo_selected(algo: str) -> bool:
    return SELECTED_ALGOS.is_checked(algo)

# for each (N, K), for each algo, 2 matrices with the first being the budget and the second being the average maximum reached
AVG_POINTS: dict[tuple[int, int], dict[str, tuple[np.ndarray[int], np.ndarray[float]]]] = {}
def construct_avg_points(n: int, k: int) -> dict[str, dict[int, int]]:
    global AVG_POINTS
    if ((n, k) in AVG_POINTS):
        return AVG_POINTS.get((n, k))
    
    all_points_per_algo: dict[str, (np.ndarray[int], np.ndarray[float])] = {}
    for algo in ALGO_KEYS:
        budget_points: np.ndarray[int] = np.array([])
        fitness_points: np.ndarray[float] = np.array([])
        
        for runinfo in file_infos[n][k][algo].values():
            budget_points = np.unique(np.concatenate((budget_points, runinfo.data.budget)))
            budget_points.sort(kind='mergesort')
        
        for runinfo in file_infos[n][k][algo].values():
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
    
    AVG_POINTS[(n, k)] = all_points_per_algo
    return all_points_per_algo
def plot_anytime_avg(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    if (X_SCALE.get_value() == "log scale"):
        ax.set_xscale('log')
        
    all_points_per_algo: dict[str, tuple[np.ndarray[int], np.ndarray[float]]] = construct_avg_points(N.get_value(), K.get_value())

    sorted_algos = sorted(ALGO_KEYS, key=lambda algo: all_points_per_algo[algo][1][-1], reverse=True)
    for algo in sorted_algos:
        if not(is_algo_selected(algo)):
            continue
        
        x, y = all_points_per_algo[algo]
        
        some_run_info = file_infos[50][0][algo][0]
        line, = ax.plot(x, y, label=some_run_info.label, color = some_run_info.color, linestyle = some_run_info.linestyle)
        legends.append(some_run_info.label)
        lines.append(line)
        
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    ax.grid()
    return lines, legends
def plot_anytime(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    lines = []
    legends = []
    if (X_SCALE.get_value() == "log scale"):
        ax.set_xscale('log')
    
    sorted_algos = sorted(ALGO_KEYS, key=lambda algo:file_infos[N.get_value()][K.get_value()][algo][I.get_value()].data.fitness_after_jump.max(), reverse=True)
    
    for algo in sorted_algos:
        if not(is_algo_selected(algo)):
            continue
        
        run_info = file_infos[N.get_value()][K.get_value()][algo][I.get_value()]
        data = run_info.data
        
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
        
        line, = ax.plot(all_x, all_y, label=run_info.label, color = run_info.color, linestyle = run_info.linestyle)
        legends.append(run_info.label)
        lines.append(line)
        
    ax.set_xlabel("budget")
    ax.set_ylabel("fitness")
    ax.grid()
    return lines, legends
def _get_single_correlation_points(algo: str, axis1: str, axis2: str) -> tuple[np.ndarray[float], np.ndarray[float]]:
    infos: list[NKRunInfo] = []
    if (I.get_value() == "avg"):
        infos = file_infos[N.get_value()][K.get_value()][algo].values()
    else:
        infos = [file_infos[N.get_value()][K.get_value()][algo][I.get_value()]]
        
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
def _plot_correlation_regression(fig: plt.Figure, ax: plt.Axes, axis1: str, axis2: str) -> tuple[list[plt.Line2D], list[str], float]:
    lines = []
    legends = []
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        algo = ALGO_KEYS[i]
        if not(is_algo_selected(algo)):
            continue
        
        all_x, all_y = _get_single_correlation_points(algo, axis1, axis2)
        max_val = max(max_val, all_x.max(), all_y.max())
        
        some_run_info = file_infos[50][0][algo][0]
        p, = ax.plot(all_x, all_y, linestyle="", marker='o', color = some_run_info.color, markersize=3, label=some_run_info.label, alpha = min(1, 150 / len(all_x)))
        legends.append("")
        lines.append(p)
        
        #region aproximation
        theta = np.polyfit(all_x, all_y, deg=REGRESSION.get_value())
        model = np.poly1d(theta)
        x = np.linspace(all_x.min(), all_x.max(), 100)
        line, = ax.plot(x, model(x), color = some_run_info.color, linestyle = some_run_info.linestyle, label = some_run_info.label + " regression")
        legends.append(some_run_info.label + " regression")
        lines.append(line)
        #endregion
    
    return lines, legends, max_val
def _plot_correlation_mean_std(fig: plt.Figure, ax: plt.Axes, axis1: str, axis2: str) -> tuple[list[plt.Line2D], list[str], float]:
    lines = []
    legends = []
    
    if (axis1.startswith("fitness") and axis2.startswith("fitness")):
        return lines, legends, 1
    
    max_val: float = 0
    for i in range(len(ALGO_KEYS)):
        algo = ALGO_KEYS[i]
        if not(is_algo_selected(algo)):
            continue
        
        all_x, all_y = _get_single_correlation_points(algo, axis1, axis2)
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
        some_run_info = file_infos[50][0][algo][0]
        if (axis1.startswith("fitness")):
            ax.fill_betweenx(discrete_values, mean - std, mean + std, alpha=.5, linewidth=0, color = some_run_info.color)
            line2, = ax.plot(mean, discrete_values, linewidth=2, color = some_run_info.color, linestyle = some_run_info.linestyle, label=some_run_info.label + " mean")
        else:
            ax.fill_between(discrete_values, mean - std, mean + std, alpha=.5, linewidth=0, color = some_run_info.color)
            line2, = ax.plot(discrete_values, mean, linewidth=2, color = some_run_info.color, linestyle = some_run_info.linestyle, label=some_run_info.label + " mean")
        
        legends.append("")
        lines.append(line1)
        legends.append(some_run_info.label + " mean")
        lines.append(line2)
    
    return lines, legends, max_val
def plot_correlation(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
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
        lines, legends, max_val = _plot_correlation_regression(fig, ax, axis1, axis2)
    else:
        lines, legends, max_val = _plot_correlation_mean_std(fig, ax, axis1, axis2)

    if (not(AXIS1.get_value() == "fitness") and not(AXIS2.get_value() == "fitness") and not(axis1.endswith("delta")) and not(axis2.endswith("delta"))):
        line, = ax.plot(np.linspace(0, max_val), np.linspace(0, max_val), color = "gray", linestyle = "--", label = "x = y")
        legends.append("x=y")
        lines.append(line)
    ax.set_xlabel(AXIS1.get_label() + (" " + AXIS1_WHEN.get_label() if AXIS1.get_label() != "jump size" else ""))
    ax.set_ylabel(AXIS2.get_label() + (" " + AXIS2_WHEN.get_label() if AXIS2.get_label() != "jump size" else ""))
    ax.grid()
    return lines, legends
def plot(fig: plt.Figure, ax: plt.Axes) -> tuple[list[plt.Line2D], list[str]]:
    if (PLOT_TYPE.get_value() == "anytime"):
        if (I.get_value() == "avg"):
            return plot_anytime_avg(fig, ax)
        else:
            return plot_anytime(fig, ax)
    return plot_correlation(fig, ax)

# region output_table
def add_spaces(value: float, str_size: int):
    return str(value) + " " * (str_size - len(str(value)))

COUNT_PER_INSTANCE: int = 2 # nb of runs to take per instance (should not exceed the number of runs done on each instances)

def generate_avg_table_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(get_label_and_style(algo)["label"], CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    
    for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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
def generate_avg_table_latex():
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
    
    for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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

def generate_budget_tables_md():
    CELL_SIZE = 16
    
    header: str = "| " + " | ".join([add_spaces(get_label_and_style(algo)["label"], CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    header += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    budgets: list[int] = [100_000, 10_000, 1_000]
    best_budget_content: list[str] = [header for _ in budgets]
    
    for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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
def generate_budget_tables_latex():
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
    
        for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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

def avg_budget_per_algo_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(get_label_and_style(algo)["label"], CELL_SIZE) for algo in ["instance (N_K)"] + ALGO_KEYS])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(ALGO_KEYS) + 1))
    
    for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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
def avg_budget_per_algo_latex():
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
    
    for n, n_data in sorted(file_infos.items(), key=lambda p : p[0]):
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

# avg_budget_per_algo_md()
# avg_budget_per_algo_latex()

# generate_avg_table_md()
# generate_avg_table_latex()

# generate_budget_tables_md()
# generate_budget_tables_latex()
# endregion

# ===============================================================================================
fig, ax = plt.subplots()
fig.subplots_adjust(left=0.045, right=0.85, bottom=0.35, top=0.95)
lines: list = []
annot: Annotation

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

# region permanent buttons ======================================================================
def update_visibility():
    for b in [AXIS1, AXIS2, CORRELATION_PLOT]:
        b.set_visible(PLOT_TYPE.get_value() == "correlation" and not(FULL_SCREEN))
    for b in [X_SCALE]:
        b.set_visible(PLOT_TYPE.get_value() != "correlation" and not(FULL_SCREEN))
    
    AXIS1_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS1.get_value() != "size_of_the_jump" and not(FULL_SCREEN))
    AXIS2_WHEN.set_visible(PLOT_TYPE.get_value() == "correlation" and AXIS2.get_value() != "size_of_the_jump" and not(FULL_SCREEN))
    
    REGRESSION.set_visible(PLOT_TYPE.get_value() == "correlation" and CORRELATION_PLOT.get_value() == "regression" and not(FULL_SCREEN))
    
    update()

def place_buttons():
    global N, K, I, PLOT_TYPE, X_SCALE, LEGEND_POSITION, SELECTED_ALGOS, AXIS1, AXIS1_WHEN, AXIS2, AXIS2_WHEN, CORRELATION_PLOT, REGRESSION
    
    N_keys = sorted(file_infos.keys())
    N = ButtonCycle(fig.add_axes([0.045, 0.2, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, update)
    K_keys = sorted(file_infos[N_keys[0]].keys())
    K = ButtonCycle(fig.add_axes([0.155, 0.2, 0.1, 0.05]), ["K" + str(i) for i in K_keys], K_keys, update)
    I_keys = sorted(file_infos[N_keys[0]][K_keys[0]][ALGO_KEYS[0]].keys())
    I = ButtonCycle(fig.add_axes([0.265, 0.2, 0.1, 0.05]), ["I" + str(i) for i in I_keys] + ["AVG"], I_keys + ["avg"], update)

    PLOT_TYPE = ButtonCycle(fig.add_axes([0.5, 0.2, 0.35, 0.05]), ["anytime", "correlation"], callback=update_visibility)

    X_SCALE = ButtonCycle(fig.add_axes([0.5, 0.14, 0.15, 0.05]), ["log scale", "linear scale"], callback=update)
    LEGEND_POSITION = ButtonCycle(fig.add_axes([0.045, 0.14, 0.1, 0.05]), ["best", "upper right", "upper left", "lower left", "lower right"], [i for i in range(5)], update)

    AXIS1 = ButtonCycle(fig.add_axes([0.66, 0.14, 0.09, 0.05]), ["jump size", "fitness", "improving neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=update_visibility)
    AXIS2 = ButtonCycle(fig.add_axes([0.66, 0.08, 0.09, 0.05]), ["fitness", "improving neighbors", "jump size"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=update_visibility)
    AXIS1_WHEN = ButtonCycle(fig.add_axes([0.76, 0.14, 0.09, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=update)
    AXIS2_WHEN = ButtonCycle(fig.add_axes([0.76, 0.08, 0.09, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=update)

    CORRELATION_PLOT = ButtonCycle(fig.add_axes([0.66, 0.02, 0.09, 0.05]), ["regression", "mean+std"], callback=update_visibility)
    REGRESSION = ButtonCycle(fig.add_axes([0.76, 0.02, 0.09, 0.05]), ["linear", "quadratic", "degree 3"], [1, 2, 3], callback=update)
    
    temp_keys, temp_values = list(zip(*sorted([(get_label_and_style(algo)["label"], algo) for algo in ALGO_KEYS], key=lambda t: t[0])))
    SELECTED_ALGOS = ButtonCheck(fig.add_axes([0.86, 0.02, 0.13, 0.96]), temp_keys, temp_values, callback=update)
place_buttons()
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
        fig.subplots_adjust(left=0.045, right=0.85, bottom=0.35, top=0.95)
        plt.rcParams.update({'font.size': 10})
    
    for b in [PLOT_TYPE, X_SCALE, LEGEND_POSITION, AXIS1, AXIS2, AXIS1_WHEN, AXIS2_WHEN, REGRESSION, SELECTED_ALGOS, N, K, I]:
        b.set_visible(not(FULL_SCREEN))
    update_visibility()
    
fig.canvas.mpl_connect('key_press_event', lambda evt: switch_full_screen(repr(evt.key)))
# fig.canvas.mpl_connect('key_press_event', lambda evt: print(repr(evt.key)))
# endregion =====================================================================================

update_visibility()

def set_default_selected_algos():
    for algo in ALGO_KEYS:
        if (algo.startswith("hc_") and "cycle" not in algo and "first" not in algo):
            SELECTED_ALGOS.check(algo)
        elif (algo.startswith("greedy_") and "fixed" not in algo and "tabu" not in algo):
            SELECTED_ALGOS.check(algo)
set_default_selected_algos()

key_held: KeyHoldEvent = KeyHoldEvent(fig)
SELECTED_ALGOS.add_key_hold_shortcuts(key_held)

plt.show()