from typing import Literal, get_args

def try_float_cast(string: str) -> float:
    try:
        v = float(string)
        return v
    except ValueError:
        print(f"can't cast {string} to a float !")
        exit(0)

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
    def from_algo(cls, algo: str):
        if (algo.startswith("greedy_tabu")):
            return None
        
        if (algo.startswith("greedy_lambda")):
            return None
        
        if (algo.startswith("tabu")):
            return None
        
        if (algo.startswith("lambda")):
            return None
        
        if (algo.startswith("greedy")):
            return GreedyAlgo.from_algo(algo)
        
        if (algo.startswith("hc")):
            return None
        
        print(f"algo file label not recognized : {algo} !")
        exit(0)
    @classmethod
    def translate_amount(cls, value: float):
        return f"{int(value * 100)}%"
    
class GreedyAlgo(Algo):
    type SCOPES = Literal['all', 'improve', 'fixed']
    type CRITERIONS = Literal['best', 'first', 'least', 'random']
    SCOPE_TRANSLATIONS: dict[SCOPES, str] = {
            "all": "full",
            "improve": "improve",
            "fixed": "top",
    }
    CRITERION_TRANSLATIONS: dict[CRITERIONS, str] = {
        "best": "best",
        "first": "maximal",
        "least": "least",
        "random": "first",
    }
    
    scope: SCOPES
    criterion: CRITERIONS
    fixed_amount: float
    
    def __init__(self, algo: str, scope: SCOPES, criterion: CRITERIONS, fixed_amount: float = .5):
        super().__init__(algo)
        self.scope = scope
        self.criterion = criterion
        self.fixed_amount = fixed_amount
    
    @classmethod
    def from_algo(cls, algo: str):
        full_algo = algo
        algo = algo.removeprefix("greedy").removeprefix("_")
        
        for scope in get_args(cls.SCOPES):
            if (algo.startswith(scope)):
                algo = algo.removeprefix(scope).removeprefix("_")
                
                for criterion in get_args(cls.CRITERIONS):
                    if (algo.startswith(criterion)):
                        algo = algo.removeprefix(criterion).removeprefix("_")
                        
                        if (scope == "fixed"):
                            return cls(full_algo, scope, criterion, try_float_cast(algo))
                        else:
                            return cls(full_algo, scope, criterion)

        print(f"malformed algo file label : {full_algo} !")
        exit(0)

    def get_plot_color(self) -> str:
        return self.REFERENCE_COLORS[self.criterion]
    def get_plot_style(self) -> str:
        return "solid" if self.scope == "all" else "dashdot"
    def get_plot_label(self) -> str:
        if (self.scope == "fixed"):
            return f"GJ {self.SCOPE_TRANSLATIONS[self.scope]}-{self.translate_amount(self.fixed_amount)} {self.CRITERION_TRANSLATIONS[self.criterion]}",
        else:
            return f"GJ {self.SCOPE_TRANSLATIONS[self.scope]} {self.CRITERION_TRANSLATIONS[self.criterion]}",
    
    def get_tex_label(self) -> str:
        return self.get_tex_category() + " " + self.get_tex_sub_category()
    def get_tex_category(self) -> str:
        return ""
    def get_tex_sub_category(self) -> str:
        return ""