import os
import pandas as pd
import numpy as np
import io

class NKRunInfo:
    _data: pd.DataFrame
    name: str
    
    N: int
    K: int
    algo: str
    instance: int
    
    label: str
    linestyle: str
    color: str
    
    @classmethod
    def translate_amount(cls, amount: str):
        try:
            v = float(amount)
            return f"{int(v * 100)}%"
        except ValueError:
            print(f"can't translate amount for {amount} !")
            exit(0)
    @classmethod
    def get_label_and_style(cls, algo: str)->dict[str, str]:
        full_algo = algo
        reference_color = {
            "best": "blue",
            "first": "orange",
            "least": "red",
            "random": "green",
        }
        
        if (algo.startswith("greedy_tabu")):
            algo = algo.removeprefix("greedy_tabu").removeprefix("_")
            
            aspiration: str = ""
            if (algo.startswith("aspiration")):
                algo = algo.removeprefix("aspiration").removeprefix("_")
                aspiration = " aspiration"
            
            push_translations = {
                "BestToWorst": "BtW",
                "BestToWorstClamped": "BtWc",
                "WorstToBest": "WtB",
                "Random": "Rand",
            }
            
            for push, push_tr in push_translations.items():
                if (algo.startswith(push+"_")):
                    algo = algo.removeprefix(push).removeprefix("_")
                    
                    tabu_size: str = algo.split("_")[0]
                    tabu_random: str = algo.split("_")[1].removeprefix("r")
                    return {
                        "label": f"GJ tabu{aspiration} {push_tr} {cls.translate_amount(tabu_size)}~{cls.translate_amount(tabu_random)}",
                        "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
                        "color": "cyan",
                    }
            print(f"malformed algo file label : {full_algo} !")
            exit(0)
        
        if (algo.startswith("greedy_lambda")):
            algo = algo.removeprefix("greedy_lambda").removeprefix("_")
            
            aspiration: str = ""
            if (algo.startswith("aspiration")):
                algo = algo.removeprefix("aspiration").removeprefix("_")
                aspiration = " aspiration"
            
            return {
                "label": f"GJ lambda{aspiration} {cls.translate_amount(algo)}",
                "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
                "color": "yellow",
            }
        
        if (algo.startswith("tabu")):
            algo = algo.removeprefix("tabu").removeprefix("_")
            
            aspiration: str = ""
            if (algo.startswith("aspiration")):
                algo = algo.removeprefix("aspiration").removeprefix("_")
                aspiration = " aspiration"
                    
            tabu_size: str = algo.split("_")[0]
            tabu_random: str = algo.split("_")[1].removeprefix("r")
            return {
                "label": f"tabu{aspiration} {cls.translate_amount(tabu_size)}~{cls.translate_amount(tabu_random)}",
                "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
                "color": "gray",
            }
        
        if (algo.startswith("lambda")):
            algo = algo.removeprefix("lambda").removeprefix("_")
            
            aspiration: str = ""
            if (algo.startswith("aspiration")):
                algo = algo.removeprefix("aspiration").removeprefix("_")
                aspiration = " aspiration"
            
            return {
                "label": f"lambda{aspiration} {cls.translate_amount(algo)}",
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
                                        "label": f"GJ {scope_tr}-{cls.translate_amount(algo)} {criterion_tr}",
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
    
    def __init__(self, n: int, k: int, algo: str, instance: str, name: str, path: str):
        self.name = name
        self.algo = algo
        self.N = n
        self.K = k
        self.instance = instance
        self.path = path
        
        label_and_style = self.get_label_and_style(algo)
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
        
        result = cls(n, k, algo, instance, name, path)
        result._data = None
        return result
    def get_data(self):
        if (self._data is not None):
            return self._data

        with open(self.path) as f: self._data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
        self._data.fitness_before_jump /= self.N
        self._data.fitness_after_jump /= self.N
        return self._data

    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"

class DataLoader:
    file_infos: dict[int, dict[int, dict[str, dict[int, NKRunInfo]]]]
    N_keys: list[str]
    K_keys: list[str]
    I_keys: list[str]
    Algo_keys: list[str]
    
    def __init__(self, rundata_path: str, small_load: bool = False):
        self.file_infos = {}
        from tqdm import tqdm
        files: list[tuple[str, str]] = []
        for d in os.walk(rundata_path+"/local_search/NK"):
            if (small_load):
                files += [(d[0], f) for f in d[2] if "N50/K0" in d[0]]
            else:
                files += [(d[0], f) for f in d[2]]

        for i in tqdm(range(len(files))):
            d, f = files[i]
            info: NKRunInfo = NKRunInfo.from_file(d + "/" + f)
            if (info != None):
                self.file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, {})[info.instance] = info

        self.N_keys = sorted(self.file_infos.keys())
        self.K_keys = sorted(self.file_infos[self.N_keys[0]].keys())
        self.Algo_keys = sorted(self.file_infos[self.N_keys[0]][self.K_keys[0]].keys())
        self.I_keys = sorted(self.file_infos[self.N_keys[0]][self.K_keys[0]][self.Algo_keys[0]].keys())
    
    def get_files(self, n: int, k: int, algo: str) -> list[NKRunInfo]:
        return self.file_infos[n][k][algo].values()
    def get_file(self, n: int, k: int, algo: str, i: int) -> NKRunInfo:
        return self.file_infos[n][k][algo][i]
    def get_reference_file(self, algo: str) -> NKRunInfo:
        return self.file_infos[self.N_keys[0]][self.K_keys[0]][algo][self.I_keys[0]]

    # for each (N, K, algo), 2 arrays with the first being the budget and the second being the average maximum reached
    _avg_points: dict[tuple[int, int, str], tuple[np.ndarray[int], np.ndarray[float]]] = {}
    def get_avg_points(self, n: int, k: int, algo: str) -> dict[int, int]:
        if ((n, k, algo) in self._avg_points):
            return self._avg_points.get((n, k, algo))
        
        budget_points: np.ndarray[int] = np.array([])
        fitness_points: np.ndarray[float] = np.array([])
        
        for runinfo in self.get_files(n, k, algo):
            budget_points = np.unique(np.concatenate((budget_points, runinfo.get_data().budget)))
            budget_points.sort(kind='mergesort')
        
        for runinfo in self.get_files(n, k, algo):
            temp: np.ndarray[float] = np.array([runinfo.get_data().budget, runinfo.get_data().fitness_after_jump])
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
        
        self._avg_points[(n, k, algo)] = (budget_points, fitness_points.mean(axis=0))
        return self._avg_points[(n, k, algo)]
