from typing import Any

import matplotlib.pyplot as plt

from button import ButtonCycle, KeyHoldEvent, ButtonCheckPartial, TextField
from data_loader import DataLoader

from plots.plot_axis import PlotAxis
from plots.plot_anytime import PlotAnytime
from plots.plot_one_run import PlotOneRun
from plots.plot_dual_correlation import PlotDualCorrelation

class Window:
    # =========== buttons =============
    FULL_SCREEN: bool = False
    PLOT_TYPE: ButtonCycle = None
    N_NK: ButtonCycle = None
    N_SAT: ButtonCycle = None
    N_QUBO: ButtonCycle = None
    K: ButtonCycle = None
    SAT_TYPE: ButtonCycle = None
    PROBLEM: ButtonCycle = None
    SELECTED_ALGOS_1: ButtonCheckPartial = None
    SELECTED_ALGOS_2: ButtonCheckPartial = None
    FILTER: TextField
    AXIS1: ButtonCycle = None
    AXIS2: ButtonCycle = None
    AXIS1_WHEN: ButtonCycle = None
    AXIS2_WHEN: ButtonCycle = None
    CORRELATION_PLOT: ButtonCycle = None
    CORRELATION_HIST: ButtonCycle = None
    REGRESSION: ButtonCycle = None
    X_SCALE: ButtonCycle = None
    LEGEND_POSITION: ButtonCycle = None
    # ==================================
    
    data_loaders: dict[str, DataLoader]
    figure: plt.Figure
    plot_axis: PlotAxis
    key_held: KeyHoldEvent
    
    kwargs: dict[str, Any]
    
    def __init__(self, data_loaders: dict[str, DataLoader], figure: plt.Figure, **kwargs):
        self.figure = figure
        self.data_loaders = data_loaders
        self.kwargs = kwargs
        
        self.figure.canvas.mpl_connect('key_press_event', lambda evt: self.on_key_press(repr(evt.key)))
        self.key_held: KeyHoldEvent = KeyHoldEvent(self.figure)
        
        self.place_buttons()
        self.set_default_selected_algos()
        self.SELECTED_ALGOS_1.add_key_hold_shortcuts(self.key_held)
        self.SELECTED_ALGOS_2.add_key_hold_shortcuts(self.key_held)
        
        self.update()
        plt.show()

    def place_buttons(self):
        self.PROBLEM = ButtonCycle(self.figure.add_axes([0.045, 0.2, 0.1, 0.05]), list(self.data_loaders.keys()), callback=self.update)
        
        N_keys = sorted(self.data_loaders["NK"].N_keys)
        self.N_NK = ButtonCycle(self.figure.add_axes([0.155, 0.2, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, self.update)
        self.N_NK.set_to(-1)
        N_keys = sorted(self.data_loaders["Sat"].N_keys)
        self.N_SAT = ButtonCycle(self.figure.add_axes([0.155, 0.2, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, self.update)
        self.N_SAT.set_to(-1)
        N_keys = sorted(self.data_loaders["Qubo"].N_keys)
        self.N_QUBO = ButtonCycle(self.figure.add_axes([0.155, 0.2, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, self.update)
        self.N_QUBO.set_to(-1)
        
        K_keys = sorted(self.data_loaders["NK"].K_keys)
        self.K = ButtonCycle(self.figure.add_axes([0.265, 0.2, 0.1, 0.05]), ["K" + str(i) for i in K_keys], K_keys, self.update)
        Sat_keys = sorted(self.data_loaders["Sat"].type_keys) if "Sat" in self.data_loaders else ["no sat"]
        self.SAT_TYPE = ButtonCycle(self.figure.add_axes([0.265, 0.2, 0.1, 0.05]), Sat_keys, callback=self.update)        
        self.K.set_to(-1)
        
        self.LEGEND_POSITION = ButtonCycle(self.figure.add_axes([0.045, 0.08, 0.1, 0.05]), ["legend position", "upper right", "upper left", "lower left", "lower right"], [i for i in range(5)], self.soft_update)
        self.X_SCALE = ButtonCycle(self.figure.add_axes([0.155, 0.08, 0.1, 0.05]), ["linear scale", "log scale"], callback=self.soft_update)

        self.PLOT_TYPE = ButtonCycle(self.figure.add_axes([0.4, 0.2, 0.32, 0.05]), ["anytime", "correlation", "one run"], callback=self.update)

        self.AXIS1 = ButtonCycle(self.figure.add_axes([0.51, 0.14, 0.1, 0.05]), ["jump size", "fitness", "improving neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=self.update)
        self.AXIS2 = ButtonCycle(self.figure.add_axes([0.51, 0.08, 0.1, 0.05]), ["fitness", "improving neighbors", "jump size"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=self.update)
        self.AXIS1_WHEN = ButtonCycle(self.figure.add_axes([0.62, 0.14, 0.1, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=self.update)
        self.AXIS2_WHEN = ButtonCycle(self.figure.add_axes([0.62, 0.08, 0.1, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=self.update)

        self.CORRELATION_PLOT = ButtonCycle(self.figure.add_axes([0.51, 0.02, 0.1, 0.05]), ["mean+std", "regression"], callback=self.update)
        self.REGRESSION = ButtonCycle(self.figure.add_axes([0.62, 0.02, 0.1, 0.05]), ["linear", "quadratic", "degree 3"], [1, 2, 3], callback=self.update)
        self.CORRELATION_HIST = ButtonCycle(self.figure.add_axes([0.4, 0.02, 0.1, 0.05]), ["correlation+hist", "correlation"], [True, False], callback=self.update)
        
        sorted_algos = sorted([(self.data_loaders["NK"].get_file(algo).algo_infos.get_full_label('plot', **self.kwargs), algo) for algo in self.data_loaders["NK"].Algo_keys], key=lambda t: t[0])
        temp_keys, temp_values = list(zip(*sorted_algos))
        half_point = len(sorted_algos) // 2
        self.SELECTED_ALGOS_1 = ButtonCheckPartial(self.figure.add_axes([0.73, 0.02, 0.12, 0.96]), temp_keys[:half_point], temp_values[:half_point], callback=self.update)
        self.SELECTED_ALGOS_2 = ButtonCheckPartial(self.figure.add_axes([0.88, 0.02, 0.12, 0.96]), temp_keys[half_point:], temp_values[half_point:], callback=self.update)
        
        def on_filter():
            filters: list[str] = self.FILTER.get_text().split()
            self.SELECTED_ALGOS_1.filter(filters)
            self.SELECTED_ALGOS_2.filter(filters)
        self.FILTER = TextField(self.figure.add_axes([0.045, 0.02, 0.32, 0.05]), "filter", on_filter)
    def set_default_selected_algos(self):
        for button in [self.SELECTED_ALGOS_1, self.SELECTED_ALGOS_2]:
            for i in range(len(button.values)):
                algo = button.values[i]
                if (algo.startswith("hc_") and "cycle" not in algo and "first" not in algo):
                    button.check_index(i)
                elif (algo.startswith("greedy_") and ("all" in algo or "improve" in algo) and ("best" in algo or "least" in algo or "first" in algo) and "guided" not in algo):
                    button.check_index(i)

    def clear_plot_axes(self):
        # Remove only axes that are NOT buttons
        for a in self.figure.axes:
            if not hasattr(a, "_is_button"):  # custom flag
                self.figure.delaxes(a)
    def draw_idle(self):
        self.figure.canvas.draw_idle()
    def soft_update(self, event = None):
        self.visibility_update(event)
        self.plot_axis.soft_update(event)
        
        self.draw_idle()
    def title_update(self):
        self.figure.canvas.manager.set_window_title(f"{self.get_problem_label()}_{self.get_plot_type_label()}")
    def update(self, event = None):
        self.clear_plot_axes()
        self.visibility_update(event)
        self.title_update()
        
        if (self.get_plot_type() == "anytime"):
            self.plot_axis = PlotAnytime(self)
        elif (self.get_plot_type() == "one run"):
            self.plot_axis = PlotOneRun(self)
        else:
            self.plot_axis = PlotDualCorrelation(self)

        self.draw_idle()
    def set_font_size(self, size: int):
        plt.rcParams.update({'font.size': size})
        for text in self.figure.findobj(match=lambda x: hasattr(x, "set_fontsize")):
            text.set_fontsize(size)
    def visibility_update(self, event = None):
        for b in [self.AXIS1, self.AXIS2, self.CORRELATION_PLOT, self.CORRELATION_HIST]:
            b.set_visible(self.PLOT_TYPE.get_value() == "correlation" and not(self.is_full_screen()))
        for b in [self.X_SCALE]:
            b.set_visible(self.PLOT_TYPE.get_value() != "correlation" and not(self.is_full_screen()))
        
        for b in [self.N_NK, self.K]:
            b.set_visible(self.get_problem() == "NK" and not(self.is_full_screen()))
        for b in [self.N_SAT, self.SAT_TYPE]:
            b.set_visible(self.get_problem() == "Sat" and not(self.is_full_screen()))
        for b in [self.N_QUBO]:
            b.set_visible(self.get_problem() == "Qubo" and not(self.is_full_screen()))
        
        self.AXIS1_WHEN.set_visible(self.PLOT_TYPE.get_value() == "correlation" and (self.AXIS1.get_value() == "nb_better_neighbors" or self.AXIS1.get_value() == "fitness") and not(self.is_full_screen()))
        self.AXIS2_WHEN.set_visible(self.PLOT_TYPE.get_value() == "correlation" and (self.AXIS2.get_value() == "nb_better_neighbors" or self.AXIS2.get_value() == "fitness") and not(self.is_full_screen()))
        
        self.REGRESSION.set_visible(self.PLOT_TYPE.get_value() == "correlation" and self.CORRELATION_PLOT.get_value() == "regression" and not(self.is_full_screen()))
        
        for b in [self.PLOT_TYPE, self.PROBLEM, self.LEGEND_POSITION, self.SELECTED_ALGOS_1, self.SELECTED_ALGOS_2, self.FILTER]:
            b.set_visible(not(self.is_full_screen()))
        if self.is_full_screen():
            self.figure.subplots_adjust(left=0.075, right=0.98, bottom=0.1, top=0.95)
            self.set_font_size(22)
        else:
            self.figure.subplots_adjust(left=0.045, right=0.72, bottom=0.35, top=0.95)
            self.set_font_size(10)
    
    def on_key_press(self, key: str):
        if (key == "' '" and self.key_held.is_key_held('shift')):
            self.FULL_SCREEN = not(self.is_full_screen())
            self.soft_update()
            return
    
    def is_full_screen(self) -> bool:
        return self.FULL_SCREEN
    def get_plot_type(self) -> str:
        return self.PLOT_TYPE.get_value()
    def get_plot_type_label(self) -> str:
        if (self.get_plot_type() == "correlation"):
            return f"{self.get_axis2_label().replace(" ", "_")}_VS_{self.get_axis1_label().replace(" ", "_")}"
        return self.get_plot_type().replace(" ", "_")
    def get_n(self) -> str:
        if (self.get_problem() == "NK"): return self.N_NK.get_value()
        if (self.get_problem() == "Sat"): return self.N_SAT.get_value()
        if (self.get_problem() == "Qubo"): return self.N_QUBO.get_value()
        return self.N_NK.get_value()
    def get_k(self) -> str:
        return self.K.get_value()
    def get_sat_type(self) -> str:
        return self.SAT_TYPE.get_value()
    def get_problem(self) -> str:
        return self.PROBLEM.get_value()
    def get_problem_label(self) -> str:
        if (self.get_problem() == "NK"):
            return f"N{self.get_n()}_K{self.get_k()}"
        if (self.get_problem() == "Sat"):
            return f"Sat_{self.get_n()}_{self.get_sat_type()}"
        if (self.get_problem() == "Qubo"):
            return f"Qubo_{self.get_n()}"
        return self.PROBLEM.get_value()
    def is_algo_selected(self, algo: str) -> bool:
        return self.SELECTED_ALGOS_1.is_checked(algo) or self.SELECTED_ALGOS_2.is_checked(algo)
    def get_selected_algos(self) -> list[str]:
        return [algo for algo in self.data_loaders["NK"].Algo_keys if self.is_algo_selected(algo)]
    def get_axis1(self) -> str:
        axis1: str = self.AXIS1.get_value()
        if (axis1 == "nb_better_neighbors" or axis1 == "fitness"):
            axis1 += self.AXIS1_WHEN.get_value()
        return axis1
    def get_axis2(self) -> str:
        axis2: str = self.AXIS2.get_value()
        if (axis2 == "nb_better_neighbors" or axis2 == "fitness"):
            axis2 += self.AXIS2_WHEN.get_value()
        return axis2
    def get_axis1_label(self) -> str:
        axis1: str = self.AXIS1.get_label()
        if (axis1 == "improving neighbors" or axis1 == "fitness"):
            if (self.AXIS1_WHEN.get_label() == "delta"):
                axis1 = self.AXIS1_WHEN.get_label() + " " + axis1
            else:
                axis1 += " " + self.AXIS1_WHEN.get_label()
        return axis1
    def get_axis2_label(self) -> str:
        axis2: str = self.AXIS2.get_label()
        if (axis2 == "improving neighbors" or axis2 == "fitness"):
            if (self.AXIS2_WHEN.get_label() == "delta"):
                axis2 = self.AXIS2_WHEN.get_label() + " " + axis2
            else:
                axis2 += " " + self.AXIS2_WHEN.get_label()
        return axis2
    def get_correlation_type(self) -> str:
        return self.CORRELATION_PLOT.get_value()
    def do_hist_plot(self) -> bool:
        return self.CORRELATION_HIST.get_value()
    def get_regression_degree(self) -> int:
        return self.REGRESSION.get_value()
    def get_x_scale(self) -> str:
        return self.X_SCALE.get_value()
    def get_legend_position(self) -> str:
        return self.LEGEND_POSITION.get_value()
    
    def get_data_loader(self) -> DataLoader:
        return self.data_loaders[self.get_problem()].set_parameters(
            n = self.get_n(),
            k = self.get_k(),
            type_name = self.get_sat_type(),
        )