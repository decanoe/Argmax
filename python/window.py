import matplotlib.pyplot as plt

from button import ButtonCycle, KeyHoldEvent, ButtonCheck
from data_loader import DataLoader

from plots.plot_axis import PlotAxis
from plots.plot_anytime import PlotAnytime
from plots.plot_anytime_avg import PlotAnytimeAvg
from plots.plot_dual_correlation import PlotDualCorrelation

class Window:
    # =========== buttons =============
    FULL_SCREEN: bool = False
    PLOT_TYPE: ButtonCycle = None
    N: ButtonCycle = None
    K: ButtonCycle = None
    I: ButtonCycle = None
    SELECTED_ALGOS_GJ: ButtonCheck = None
    SELECTED_ALGOS_HC: ButtonCheck = None
    AXIS1: ButtonCycle = None
    AXIS2: ButtonCycle = None
    AXIS1_WHEN: ButtonCycle = None
    AXIS2_WHEN: ButtonCycle = None
    CORRELATION_PLOT: ButtonCycle = None
    REGRESSION: ButtonCycle = None
    X_SCALE: ButtonCycle = None
    LEGEND_POSITION: ButtonCycle = None
    # ==================================
    
    data_loader: DataLoader
    figure: plt.Figure
    plot_axis: PlotAxis
    key_held: KeyHoldEvent
    
    def __init__(self, data_loader: DataLoader, figure: plt.Figure):
        self.figure = figure
        self.data_loader = data_loader
        
        self.figure.canvas.mpl_connect('key_press_event', lambda evt: self.on_key_press(repr(evt.key)))
        self.key_held: KeyHoldEvent = KeyHoldEvent(self.figure)
        
        self.place_buttons()
        self.set_default_selected_algos()
        self.SELECTED_ALGOS_HC.add_key_hold_shortcuts(self.key_held)
        self.SELECTED_ALGOS_GJ.add_key_hold_shortcuts(self.key_held)
        
        self.update()
        plt.show()

    def place_buttons(self):
        N_keys = sorted(self.data_loader.N_keys)
        self.N = ButtonCycle(self.figure.add_axes([0.045, 0.2, 0.1, 0.05]), ["N" + str(i) for i in N_keys], N_keys, self.update)
        K_keys = sorted(self.data_loader.K_keys)
        self.K = ButtonCycle(self.figure.add_axes([0.155, 0.2, 0.1, 0.05]), ["K" + str(i) for i in K_keys], K_keys, self.update)
        I_keys = sorted(self.data_loader.I_keys)
        self.I = ButtonCycle(self.figure.add_axes([0.265, 0.2, 0.1, 0.05]), ["I" + str(i) for i in I_keys] + ["avg"], I_keys + ["avg"], self.update)
        
        self.N.set_to(-1)
        self.K.set_to(-1)
        self.I.set_to(-1)

        self.LEGEND_POSITION = ButtonCycle(self.figure.add_axes([0.045, 0.14, 0.1, 0.05]), ["best", "upper right", "upper left", "lower left", "lower right"], [i for i in range(5)], self.soft_update)

        self.PLOT_TYPE = ButtonCycle(self.figure.add_axes([0.4, 0.2, 0.32, 0.05]), ["anytime", "correlation"], callback=self.update)
        self.X_SCALE = ButtonCycle(self.figure.add_axes([0.4, 0.14, 0.1, 0.05]), ["log scale", "linear scale"], callback=self.soft_update)

        self.AXIS1 = ButtonCycle(self.figure.add_axes([0.51, 0.14, 0.1, 0.05]), ["jump size", "fitness", "improving neighbors"], ["size_of_the_jump", "fitness", "nb_better_neighbors"], callback=self.update)
        self.AXIS2 = ButtonCycle(self.figure.add_axes([0.51, 0.08, 0.1, 0.05]), ["fitness", "improving neighbors", "jump size"], ["fitness", "nb_better_neighbors", "size_of_the_jump"], callback=self.update)
        self.AXIS1_WHEN = ButtonCycle(self.figure.add_axes([0.62, 0.14, 0.1, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=self.update)
        self.AXIS2_WHEN = ButtonCycle(self.figure.add_axes([0.62, 0.08, 0.1, 0.05]), ["after jump", "before jump", "delta"], ["_after_jump", "_before_jump", "_delta"], callback=self.update)

        self.CORRELATION_PLOT = ButtonCycle(self.figure.add_axes([0.51, 0.02, 0.1, 0.05]), ["mean+std", "regression"], callback=self.update)
        self.REGRESSION = ButtonCycle(self.figure.add_axes([0.62, 0.02, 0.1, 0.05]), ["linear", "quadratic", "degree 3"], [1, 2, 3], callback=self.update)
        
        sorted_algos = sorted([(self.data_loader.get_reference_file(algo).label, algo) for algo in self.data_loader.Algo_keys], key=lambda t: t[0])
        temp_keys, temp_values = list(zip(*[algo for algo in sorted_algos if algo[0].startswith("GJ")]))
        self.SELECTED_ALGOS_GJ = ButtonCheck(self.figure.add_axes([0.73, 0.02, 0.12, 0.96]), temp_keys, temp_values, callback=self.update)
        
        temp_keys, temp_values = list(zip(*[algo for algo in sorted_algos if not(algo[0].startswith("GJ"))]))
        self.SELECTED_ALGOS_HC = ButtonCheck(self.figure.add_axes([0.88, 0.02, 0.12, 0.96]), temp_keys, temp_values, callback=self.update)
    def set_default_selected_algos(self):
        for algo in self.data_loader.Algo_keys:
            if (algo.startswith("hc_") and "cycle" not in algo and "first" not in algo):
                self.SELECTED_ALGOS_HC.check(algo)
            elif (algo.startswith("greedy_") and "fixed" not in algo and "tabu" not in algo and "lambda" not in algo):
                self.SELECTED_ALGOS_GJ.check(algo)

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
    def update(self, event = None):
        self.clear_plot_axes()
        self.visibility_update(event)
        
        if (self.get_plot_type() == "anytime"):
            if (self.get_i() == "avg"):
                self.plot_axis = PlotAnytimeAvg(self)
            else:
                self.plot_axis = PlotAnytime(self)
        else:
            self.plot_axis = PlotDualCorrelation(self)

        self.draw_idle()
    def set_font_size(self, size: int):
        plt.rcParams.update({'font.size': size})
        for text in self.figure.findobj(match=lambda x: hasattr(x, "set_fontsize")):
            text.set_fontsize(size)
    def visibility_update(self, event = None):
        for b in [self.AXIS1, self.AXIS2, self.CORRELATION_PLOT]:
            b.set_visible(self.PLOT_TYPE.get_value() == "correlation" and not(self.is_full_screen()))
        for b in [self.X_SCALE]:
            b.set_visible(self.PLOT_TYPE.get_value() != "correlation" and not(self.is_full_screen()))
        
        self.AXIS1_WHEN.set_visible(self.PLOT_TYPE.get_value() == "correlation" and self.AXIS1.get_value() != "size_of_the_jump" and not(self.is_full_screen()))
        self.AXIS2_WHEN.set_visible(self.PLOT_TYPE.get_value() == "correlation" and self.AXIS2.get_value() != "size_of_the_jump" and not(self.is_full_screen()))
        
        self.REGRESSION.set_visible(self.PLOT_TYPE.get_value() == "correlation" and self.CORRELATION_PLOT.get_value() == "regression" and not(self.is_full_screen()))
        
        for b in [self.PLOT_TYPE, self.LEGEND_POSITION, self.SELECTED_ALGOS_GJ, self.SELECTED_ALGOS_HC, self.N, self.K, self.I]:
            b.set_visible(not(self.is_full_screen()))
            self.set_font_size(22)
        if self.is_full_screen():
            self.figure.subplots_adjust(left=0.075, right=0.98, bottom=0.1, top=0.95)
        else:
            self.figure.subplots_adjust(left=0.045, right=0.72, bottom=0.35, top=0.95)
            self.set_font_size(10)
    
    def on_key_press(self, key: str):
        if (key == "' '"):
            self.FULL_SCREEN = not(self.is_full_screen())
            self.soft_update()
            return
    
    def is_full_screen(self) -> bool:
        return self.FULL_SCREEN
    def get_plot_type(self) -> str:
        return self.PLOT_TYPE.get_value()
    def get_n(self) -> str:
        return self.N.get_value()
    def get_k(self) -> str:
        return self.K.get_value()
    def get_i(self) -> str:
        return self.I.get_value()
    def is_algo_selected(self, algo: str) -> bool:
        return self.SELECTED_ALGOS_GJ.is_checked(algo) or self.SELECTED_ALGOS_HC.is_checked(algo)
    def get_axis1(self) -> str:
        axis1: str = self.AXIS1.get_value()
        if (axis1 != "size_of_the_jump"):
            axis1 += self.AXIS1_WHEN.get_value()
        return axis1
    def get_axis2(self) -> str:
        axis2: str = self.AXIS2.get_value()
        if (axis2 != "size_of_the_jump"):
            axis2 += self.AXIS2_WHEN.get_value()
        return axis2
    def get_axis1_label(self) -> str:
        axis1: str = self.AXIS1.get_label()
        if (axis1 != "jump size"):
            axis1 += " " + self.AXIS1_WHEN.get_label()
        return axis1
    def get_axis2_label(self) -> str:
        axis2: str = self.AXIS2.get_label()
        if (axis2 != "jump size"):
            axis2 += " " + self.AXIS2_WHEN.get_label()
        return axis2
    def get_correlation_type(self) -> str:
        return self.CORRELATION_PLOT.get_value()
    def get_regression_degree(self) -> int:
        return self.REGRESSION.get_value()
    def get_x_scale(self) -> str:
        return self.X_SCALE.get_value()
    def get_legend_position(self) -> str:
        return self.LEGEND_POSITION.get_value()
    
    def get_data_loader(self) -> DataLoader:
        return self.data_loader