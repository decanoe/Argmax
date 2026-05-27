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
def get_int(full_algo: str, algo: str, prefix: str = '') -> tuple[str, int]:
    if (prefix != '' and not algo.startswith(prefix)):
        print(f"malformed algo file label : {full_algo} !\nexpected a int value prefixed by", prefix)
        exit(1)
    
    try:
        algo = algo.removeprefix(prefix)
        return "_".join(algo.split("_")[1:]), int(algo.split("_")[0])
    except ValueError:
        print(f"malformed algo file label : {full_algo} !\nexpected a int value")
        exit(1)
def get_bool(full_algo: str, algo: str, literal: str) -> tuple[str, bool]:
    return algo.removeprefix(literal).removeprefix("_"), algo.startswith(literal)
def get_str(full_algo: str, algo: str, prefix: str = '') -> tuple[str, str]:
    if (prefix != '' and not algo.startswith(prefix)):
        print(f"malformed algo file label : {full_algo} !\nexpected a string value prefixed by", prefix)
        exit(1)
    
    algo = algo.removeprefix(prefix)
    return "_".join(algo.split("_")[1:]), algo.split("_")[0]

def translate_amount(value: float, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs):
    if lang == 'tex': return f"${int(value * 100)}\\%$"
    return f"{int(value * 100)}%"

SCOPES = Literal['all', 'improve', 'fixed', 'adaptative']
SCOPE_TRANSLATIONS: dict[SCOPES, str] = {
    "all": "full",
    "improve": "improve",
    "fixed": "top",
}
SCOPE_TEX_TRANSLATIONS: dict[SCOPES, str] = {
    "all": "\\Full",
    "improve": "\\Improve",
    "fixed": "\\Top",
}
def translate_scope(scope: SCOPES, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
    if lang == 'tex': return SCOPE_TEX_TRANSLATIONS[scope]
    return SCOPE_TRANSLATIONS[scope]

CRITERIONS = Literal['best', 'first', 'least', 'random', 'cycle', 'middle2', 'middle', 'median']
CRITERION_TRANSLATIONS: dict[CRITERIONS, str] = {
    'best': "best",
    'first': "maximal",
    'least': "least",
    'random': "first",
    'middle': "middle score",
    'middle2': "middle jump",
    'median': "median score",
}
CRITERION_TEX_TRANSLATIONS: dict[CRITERIONS, str] = {
    'best': "\\Best",
    'first': "\\Maximal",
    'least': "\\Least",
    'random': "\\First",
    'middle': "\\MiddleScore",
    'middle2': "\\MiddleJump",
    'median': "\\MedianScore",
}
CRITERION_HC_TRANSLATIONS: dict[CRITERIONS, str] = {
    "best": "best",
    "first": "first(index based)",
    "least": "least",
    "random": "first",
    "cycle": "cycle",
}
CRITERION_HC_TEX_TRANSLATIONS: dict[CRITERIONS, str] = {
    'best': "\\Best",
    'first': "\\First_Index",
    'least': "\\Least",
    'random': "\\First",
    'cycle': "\\First_Cycle",
}
def translate_criterion_gj(criterion: CRITERIONS, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
    if lang == 'tex': return CRITERION_TEX_TRANSLATIONS[criterion]
    return CRITERION_TRANSLATIONS[criterion]
def translate_criterion_hc(criterion: CRITERIONS, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
    if lang == 'tex': return CRITERION_HC_TEX_TRANSLATIONS[criterion]
    return CRITERION_HC_TRANSLATIONS[criterion]

ORDERING = Literal['Desc', 'Asc', 'Rand']
ORDERING_TRANSLATIONS: dict[ORDERING, str] = {
    "Desc": "Desc",
    "Asc": "Asc",
    "Rand": "Rand",
}
def translate_ordering(ordering: ORDERING, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
    return ORDERING_TRANSLATIONS[ordering]

PUSH_ORDER = Literal['BestToWorst', 'BestToWorstClamped', 'WorstToBest', 'Random']
PUSH_ORDER_TRANSLATIONS = {
    "BestToWorst": "BtW",
    "BestToWorstClamped": "BtWc",
    "WorstToBest": "WtB",
    "Random": "Rand",
}
def translate_push_order(ordering: PUSH_ORDER, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
    return PUSH_ORDER_TRANSLATIONS[ordering]

class Algo:
    REFERENCE_COLORS: dict[str, str] = {
        "best": "blue",
        "first": "orange",
        "least": "red",
        "random": "green",
        "middle": "magenta",
        "middle2": "magenta",
        "median": "magenta",
    }
    
    algo: str
    def __init__(self, algo: str):
        self.algo = algo
    
    def get_algo(self) -> str:
        return self.algo
    
    def get_color(self) -> str:
        return "gray"
    def get_style(self) -> str:
        return "solid"
    
    def get_category_sort_index(self, **kwargs) -> int:
        return 999
    def get_category(self, lang: Literal['md', 'tex'], **kwargs) -> str:
        if lang == 'md': return self.get_md_category(**kwargs)
        else: return self.get_tex_category(**kwargs)
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return ""
    def get_full_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return f"{self.get_category(lang, **kwargs)} {self.get_label(lang, **kwargs)}"
    
    def is_valid(self, **kwargs) -> bool:
        return True
    
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
            return GreedyJumperAlgo(algo)
        
        if (algo.startswith("hc")):
            return HillClimberAlgo(algo)
        
        print(f"algo file label not recognized : {algo} !")
        exit(0)
class GreedyAlgo(Algo):
    positive_ordering: ORDERING
    negative_ordering: ORDERING
    
    def __init__(self, algo):
        super().__init__(algo)

    def temp_order(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
        temp: str = ""
        if not kwargs.get("display_ordering", False): return temp
        if hasattr(self, "positive_ordering"): temp += f"{translate_ordering(self.positive_ordering, lang, **kwargs)}+"
        if hasattr(self, "negative_ordering"): temp += f" {translate_ordering(self.negative_ordering, lang, **kwargs)}-"
        return temp

    def is_valid(self, **kwargs) -> bool:
        if not kwargs.get("display_ordering", False):
            if hasattr(self, "positive_ordering") and self.positive_ordering != 'Desc': return False
            if hasattr(self, "negative_ordering") and self.negative_ordering != 'Desc': return False
        return super().is_valid(**kwargs)
    
class GreedyJumperAlgo(GreedyAlgo):
    scope: SCOPES
    criterion: CRITERIONS
    fixed_amount: float
    adaptative_formula: str
    
    guided: bool
    guide_criterion: CRITERIONS
    guide_turn_count: int
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _             = get_literal(self.algo, algo, Literal['greedy'])
        algo, self.scope    = get_literal(self.algo, algo, SCOPES)
        if (self.scope == 'adaptative'): algo, self.adaptative_formula = get_str(self.algo, algo)
        
        algo, self.criterion = get_literal(self.algo, algo, CRITERIONS)
        algo, self.guided = get_bool(self.algo, algo, "guided")
        if (self.guided): algo, self.guide_criterion = get_literal(self.algo, algo, CRITERIONS)
        if (self.guided): algo, self.guide_turn_count = get_int(self.algo, algo)
        
        if (self.scope == 'fixed'): algo, self.fixed_amount = get_amount(self.algo, algo)
        
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        if (self.scope != 'improve'): algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def temp_scope(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
        if self.guided and not kwargs.get("_ignore_guided", False):
            temp: str = self.temp_scope(lang, **kwargs, _ignore_guided = True)
            if lang == 'tex': return f"{temp} \\Guided\\ ${self.guide_turn_count}$"
            else: return f"{temp} Guided {self.guide_turn_count}"
        
        match self.scope:
            case "fixed":
                return f"{translate_scope(self.scope, lang, **kwargs)}-{translate_amount(self.fixed_amount, lang, **kwargs)}"
            case "adaptative":
                if lang == 'tex': return f"${self.adaptative_formula.replace("max", "\\max")}$"
                return self.adaptative_formula
            case _:
                return f"{translate_scope(self.scope, lang, **kwargs)}"
    def temp_criterion(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs)-> str:
        return translate_criterion_gj(self.criterion, lang, **kwargs)
    
    def get_color(self) -> str:
        return self.REFERENCE_COLORS[self.criterion]
    def get_style(self) -> str:
        return "solid" if self.scope == "all" else "dashdot"
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return f"{self.temp_criterion(lang, **kwargs)} {self.temp_order(lang, **kwargs)}"
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return self.temp_scope(lang, **kwargs) 
    def get_category_sort_index(self, **kwargs) -> int:
        result: int = 1
        if self.scope in ["adaptative", "fixed"]: result += 1
        if self.guided: result += 10
        return result
class HillClimberAlgo(Algo):
    criterion: CRITERIONS
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _              = get_literal(self.algo, algo, Literal['hc'])
        algo, self.criterion = get_literal(self.algo, algo, CRITERIONS)

    def get_color(self) -> str:
        return self.REFERENCE_COLORS.get(self.criterion, 'gray')
    def get_style(self) -> str:
        return "dashed"
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return translate_criterion_hc(self.criterion, lang, **kwargs)
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return "\\HC" if lang == 'tex' else "HC"
    def get_category_sort_index(self, **kwargs) -> int:
        return 0

class GreedyTabuAlgo(GreedyAlgo):
    aspiration: bool
    push_order: PUSH_ORDER
    tabu_size: float
    tabu_random: float
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['greedy_tabu'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.push_order   = get_literal(self.algo, algo, PUSH_ORDER)
        algo, self.tabu_size    = get_amount(self.algo, algo)
        algo, self.tabu_random  = get_amount(self.algo, algo, 'r')
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def get_color(self) -> str:
        return "cyan"
    def get_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)) # densely dashdotdotted (double dot)
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        temp: str = translate_amount(self.tabu_size, lang, **kwargs)
        if self.tabu_random != 0: temp += f" {translate_amount(self.tabu_random, lang, **kwargs)}"
        temp += f" {self.temp_order(**kwargs)}"
        return temp
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        temp: str = "\\GJTabu" if lang == 'tex' else "GJ tabu"
        if self.aspiration and kwargs.get("display_aspiration", False): temp += " aspiration"
        if kwargs.get("display_push_order", False): temp += f" {translate_push_order(self.push_order, lang, **kwargs)}"
        return temp
    def get_category_sort_index(self, **kwargs) -> int:
        return 101
    
    def is_valid(self, **kwargs) -> bool:
        if not kwargs.get("display_aspiration", False):
            if not self.aspiration: return False
        if not kwargs.get("display_push_order", False):
            if self.push_order != 'WorstToBest': return False
        return super().is_valid(**kwargs)
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

    def get_color(self) -> str:
        return "gray"
    def get_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)) # densely dashdotdotted (double dot)
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        temp: str = translate_amount(self.tabu_size, lang, **kwargs)
        if self.tabu_random != 0: temp += f" {translate_amount(self.tabu_random, lang, **kwargs)}"
        return temp
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return "\\Tabu" if lang == 'tex' else "tabu"
    def get_category_sort_index(self, **kwargs) -> int:
        return 100

    def is_valid(self, **kwargs) -> bool:
        if not kwargs.get("display_aspiration", False):
            if not self.aspiration: return False
        return super().is_valid(**kwargs)

class GreedyLambdaAlgo(GreedyAlgo):
    aspiration: bool
    lambda_: float
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['greedy_lambda'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.lambda_  = get_amount(self.algo, algo)
        algo, self.positive_ordering = get_literal(self.algo, algo, ORDERING, '+')
        algo, self.negative_ordering = get_literal(self.algo, algo, ORDERING, '-')

    def get_color(self) -> str:
        return "yellow"
    def get_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)) # densely dashdotdotted (double dot)
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return f"{translate_amount(self.lambda_, lang, **kwargs)} {self.temp_order(**kwargs)}"
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        temp: str = "\\GJ_One_Lambda" if lang == 'tex' else "GJ lambda"
        if self.aspiration and kwargs.get("display_aspiration", False): temp += " aspiration"
        return temp
    def get_category_sort_index(self, **kwargs) -> int:
        return 201

    def is_valid(self, **kwargs) -> bool:
        if not kwargs.get("display_aspiration", False):
            if not self.aspiration: return False
        return super().is_valid(**kwargs)
class LambdaAlgo(Algo):
    aspiration: bool
    lambda_: float
    
    def __init__(self, algo):
        super().__init__(algo)
        algo, _         = get_literal(self.algo, algo, Literal['lambda'])
        algo, self.aspiration   = get_bool(self.algo, algo, "aspiration")
        algo, self.lambda_  = get_amount(self.algo, algo)

    def get_color(self) -> str:
        return "gray"
    def get_style(self) -> str:
        return (0, (3, 1, 1, 1, 1, 1)) # densely dashdotdotted (double dot)
    
    def get_label(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        return f"{translate_amount(self.lambda_, lang, **kwargs)}"
    def get_category(self, lang: Literal['md', 'tex', 'plot'] = 'plot', **kwargs) -> str:
        temp: str = "\\One_Lambda" if lang == 'tex' else "lambda"
        if self.aspiration and kwargs.get("display_aspiration", False): temp += " aspiration"
        return temp
    def get_category_sort_index(self, **kwargs) -> int:
        return 200

    def is_valid(self, **kwargs) -> bool:
        if not kwargs.get("display_aspiration", False):
            if not self.aspiration: return False
        return super().is_valid(**kwargs)