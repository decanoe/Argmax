import os
from data_loader import DataLoader
import matplotlib.pyplot as plt
from window import Window

if __name__ != "__main__":
    exit()
dir_path = os.path.dirname(os.path.realpath(__file__))

plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

data_loader: DataLoader = DataLoader(dir_path+"/../rundata", small_load=False)
fig, _ = plt.subplots()
Window(data_loader, fig)

"""
def _get_single_correlation_points(algo: str, axis1: str, axis2: str) -> tuple[np.ndarray[float], np.ndarray[float]]:
    infos: list[NKRunInfo] = []
    if (I.get_value() == "avg"):
        infos = data_loader.get_files(N.get_value(), K.get_value(), algo)
    else:
        infos = [data_loader.get_file(N.get_value(), K.get_value(), algo, I.get_value())]
        
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
def _plot_correlation_regression(fig: plt.Figure, axis1: str, axis2: str) -> tuple[list[AnnotedAxis], float]:
    ax = fig.add_subplot(111)
    annoted_axis: AnnotedAxis = AnnotedAxis(ax, FULL_SCREEN, LEGEND_POSITION.get_value())
    lines = []
    legends = []
    
    max_val: float = 0
    for i in range(len(data_loader.Algo_keys)):
        algo = data_loader.Algo_keys[i]
        if not(is_algo_selected(algo)):
            continue
        
        all_x, all_y = _get_single_correlation_points(algo, axis1, axis2)
        max_val = max(max_val, all_x.max(), all_y.max())
        
        some_run_info = data_loader.get_reference_file(algo)
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
    
    annoted_axis.add_lines(lines, legends)
    return [annoted_axis], max_val
def _plot_correlation_mean_std(fig: plt.Figure, axis1: str, axis2: str) -> tuple[list[AnnotedAxis], float]:
    ax = fig.add_subplot(111)
    annoted_axis: AnnotedAxis = AnnotedAxis(ax, FULL_SCREEN, LEGEND_POSITION.get_value())
    lines = []
    legends = []
    
    if (axis1.startswith("fitness") and axis2.startswith("fitness")):
        return lines, legends, 1
    
    max_val: float = 0
    for i in range(len(data_loader.Algo_keys)):
        algo = data_loader.Algo_keys[i]
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
        some_run_info = data_loader.get_reference_file(algo)
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
    
    annoted_axis.add_lines(lines, legends)
    return [annoted_axis], max_val
def plot_correlation(fig: plt.Figure) -> list[AnnotedAxis]:
    axis1: str = AXIS1.get_value()
    if (axis1 != "size_of_the_jump"):
        axis1 += AXIS1_WHEN.get_value()
    axis2: str = AXIS2.get_value()
    if (axis2 != "size_of_the_jump"):
        axis2 += AXIS2_WHEN.get_value()
    
    result: list[AnnotedAxis]
    max_val = 1
    if (CORRELATION_PLOT.get_value() == "regression"):
        result, max_val = _plot_correlation_regression(fig, axis1, axis2)
    else:
        result, max_val = _plot_correlation_mean_std(fig, axis1, axis2)

    if (not(AXIS1.get_value() == "fitness") and not(AXIS2.get_value() == "fitness") and not(axis1.endswith("delta")) and not(axis2.endswith("delta"))):
        line, = result[0].axis.plot(np.linspace(0, max_val), np.linspace(0, max_val), color = "gray", linestyle = "--", label = "x = y")
        result[0].add_lines(line, "x=y")
    
    result[0].axis.set_xlabel(AXIS1.get_label() + (" " + AXIS1_WHEN.get_label() if AXIS1.get_label() != "jump size" else ""))
    result[0].axis.set_ylabel(AXIS2.get_label() + (" " + AXIS2_WHEN.get_label() if AXIS2.get_label() != "jump size" else ""))
    result[0].axis.grid()
    return result
"""