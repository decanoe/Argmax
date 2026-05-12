from __future__ import annotations
from typing import Any, Literal, get_args

def get_literal(full_algo: str, algo: str, literals: type, suffix: str = '') -> tuple[str, Any]:
    for literal in get_args(literals):
        if (algo.startswith(literal + suffix)): return algo.removeprefix(literal + suffix).removeprefix("_"), literal
    
    print(f"malformed algo file label : {full_algo} !\nexpected a value from", [l + suffix for l in get_args(literals)])
    exit(1)
def get_amount(full_algo: str, algo: str, prefix: str = '') -> tuple[str, float]:
    if (prefix != '' and not algo.startswith(prefix)):
        print(f"malformed algo file label : {full_algo} !\nexpected a float value prefixed by", prefix)
        exit(1)
    
    try:
        algo = algo.removeprefix(prefix)
        return "_".join(algo.split("_")[1:]), float(algo.split("_")[0])
    except ValueError:
        print(f"malformed algo file label : {full_algo} !\nexpected a float value")
        exit(1)
def get_bool(full_algo: str, algo: str, literal: str) -> tuple[str, bool]:
    return algo.removeprefix(literal).removeprefix("_"), algo.startswith(literal)

SCOPES = Literal['all', 'improve', 'fixed', 'adaptative']
SCOPE_TRANSLATIONS: dict[SCOPES, str] = {
    "all": "full",
    "improve": "improve",
    "fixed": "top",
    'adaptative': "2n"
}
CRITERIONS = Literal['best', 'first', 'least', 'random', 'cycle']
CRITERION_TRANSLATIONS: dict[CRITERIONS, str] = {
    "best": "best",
    "first": "maximal",
    "least": "least",
    "random": "first",
}
CRITERION_HC_TRANSLATIONS: dict[CRITERIONS, str] = {
    "best": "best",
    "first": "first(index based)",
    "least": "least",
    "random": "first",
    "cycle": "cycle",
}
ORDERING = Literal['Desc', 'Asc', 'Rand']
ORDERING_TRANSLATIONS: dict[ORDERING, str] = {
    "Desc": "Desc",
    "Asc": "Asc",
    "Rand": "Rand",
}
PUSH_ORDER = Literal['BestToWorst', 'BestToWorstClamped', 'WorstToBest', 'Random']
PUSH_ORDER_TRANSLATIONS = {
    "BestToWorst": "BtW",
    "BestToWorstClamped": "BtWc",
    "WorstToBest": "WtB",
    "Random": "Rand",
}

class Algo:
    REFERENCE_COLORS: dict[str, str] = {
        "best": "blue",
        "first": "orange",
        "least": "red",
        "random": "green",
    }
    
    algo: str
    def __init__(self, algo: str):
        self.algo = algo
    
    def get_algo(self) -> str:
        return self.algo
    
    def get_plot_color(self) -> str:
        return "gray"
    def get_plot_style(self) -> str:
        return "solid"
    def get_plot_label(self) -> str:
        return ""
    
    def get_tex_label(self) -> str:
        return self.get_tex_category() + " " + self.get_tex_sub_category()
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""

    @classmethod
    def from_algo(cls, algo: str) -> Algo:
        if (algo.startswith("greedy_tabu")):
            return GreedyTabuAlgo(algo)
        
        if (algo.startswith("greedy_lambda")):
            return GreedyLambdaAlgo(algo)
        
        if (algo.startswith("tabu")):
            return TabuAlgo(algo)
        
        if (algo.startswith("lambda")):
            return LambdaAlgo(algo)
        
        if (algo.startswith("greedy")):
            return GreedyAlgo(algo)
        
        if (algo.startswith("hc")):
            return HillClimberAlgo(algo)
        
        print(f"algo file label not recognized : {algo} !")
        exit(0)
    @classmethod
    def translate_amount(cls, value: float):
        return f"{int(value * 100)}%"
    
class GreedyAlgo(Algo):
    scope: SCOPES
    criterion: CRITERIONS
    fixed_amount: float
    positive_ordering: ORDERING
    negative_ordering: ORDERING
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['greedy'])
        algo, self.scope     = get_literal(self.algo, algo, SCOPES)
        algo, self.criterion = get_literal(self.algo, algo, CRITERIONS)
        if (self.scope == 'fixed'): algo, self.fixed_amount = get_amount(self.algo, algo)
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        if (self.scope != 'improve'): algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def get_plot_color(self) -> str:
        return self.REFERENCE_COLORS[self.criterion]
    def get_plot_style(self) -> str:
        return "solid" if self.scope == "all" else "dashdot"
    def get_plot_label(self) -> str:
        def temp_scope()-> str:
            if (self.scope == "fixed"): return f"{SCOPE_TRANSLATIONS[self.scope]}-{self.translate_amount(self.fixed_amount)}"
            return f"{SCOPE_TRANSLATIONS[self.scope]}"
        def temp_order()-> str:
            if (self.scope == "improve"): return f"{ORDERING_TRANSLATIONS[self.positive_ordering]}+"
            return f"{ORDERING_TRANSLATIONS[self.positive_ordering]}+ {ORDERING_TRANSLATIONS[self.positive_ordering]}-"
        
        return f"GJ {temp_scope()} {CRITERION_TRANSLATIONS[self.criterion]} {temp_order()}"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""
class HillClimberAlgo(Algo):
    criterion: CRITERIONS
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _              = get_literal(self.algo, algo, Literal['hc'])
        algo, self.criterion = get_literal(self.algo, algo, CRITERIONS)

    def get_plot_color(self) -> str:
        return self.REFERENCE_COLORS.get(self.criterion, 'gray')
    def get_plot_style(self) -> str:
        return "dashed"
    def get_plot_label(self) -> str:
        return f"HC {CRITERION_HC_TRANSLATIONS[self.criterion]}"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""

class GreedyTabuAlgo(Algo):
    aspiration: bool
    push_order: PUSH_ORDER
    tabu_size: float
    tabu_random: float
    positive_ordering: ORDERING
    negative_ordering: ORDERING
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['greedy_tabu'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.push_order   = get_literal(self.algo, algo, PUSH_ORDER)
        algo, self.tabu_size    = get_amount(self.algo, algo)
        algo, self.tabu_random  = get_amount(self.algo, algo, 'r')
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def get_plot_color(self) -> str:
        return "cyan"
    def get_plot_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
    def get_plot_label(self) -> str:
        def temp_asp():
            if (self.aspiration): return "tabu aspiration"
            return "tabu"

        return f"GJ {temp_asp()} {PUSH_ORDER_TRANSLATIONS[self.push_order]} {self.translate_amount(self.tabu_size)}~{self.translate_amount(self.tabu_random)} {ORDERING_TRANSLATIONS[self.positive_ordering]}+ {ORDERING_TRANSLATIONS[self.positive_ordering]}-"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""
class TabuAlgo(Algo):
    aspiration: bool
    tabu_size: float
    tabu_random: float
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['tabu'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.tabu_size    = get_amount(self.algo, algo)
        algo, self.tabu_random  = get_amount(self.algo, algo, 'r')

    def get_plot_color(self) -> str:
        return "gray"
    def get_plot_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
    def get_plot_label(self) -> str:
        def temp_asp():
            if (self.aspiration): return "tabu aspiration"
            return "tabu"

        return f"GJ {temp_asp()} {self.translate_amount(self.tabu_size)}~{self.translate_amount(self.tabu_random)}"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""

class GreedyLambdaAlgo(Algo):
    aspiration: bool
    lambda_: float
    positive_ordering: ORDERING
    negative_ordering: ORDERING
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['greedy_lambda'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.lambda_  = get_amount(self.algo, algo)
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def get_plot_color(self) -> str:
        return "yellow"
    def get_plot_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
    def get_plot_label(self) -> str:
        def temp_asp():
            if (self.aspiration): return "lambda aspiration"
            return "lambda"

        return f"GJ {temp_asp()} {self.translate_amount(self.lambda_)} {ORDERING_TRANSLATIONS[self.positive_ordering]}+ {ORDERING_TRANSLATIONS[self.positive_ordering]}-"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""
class LambdaAlgo(Algo):
    aspiration: bool
    lambda_: float
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['lambda'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.lambda_  = get_amount(self.algo, algo)

    def get_plot_color(self) -> str:
        return "gray"
    def get_plot_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
    def get_plot_label(self) -> str:
        def temp_asp():
            if (self.aspiration): return "lambda aspiration"
            return "lambda"

        return f"one {temp_asp()} {self.translate_amount(self.lambda_)}"
    
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""
