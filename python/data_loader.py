import os
import re
import pandas as pd
import numpy as np
import io

# class NKRunInfo:
#     _data: pd.DataFrame
#     name: str
    
#     N: int
#     K: int
#     algo: str
#     instance: int
    
#     label: str
#     linestyle: str
#     color: str
    
#     @classmethod
#     def translate_amount(cls, amount: str):
#         try:
#             v = float(amount)
#             return f"{int(v * 100)}%"
#         except ValueError:
#             print(f"can't translate amount for {amount} !")
#             exit(0)
#     @classmethod
#     def get_label_and_style(cls, algo: str)->dict[str, str]:
#         full_algo = algo
#         reference_color = {
#             "best": "blue",
#             "first": "orange",
#             "least": "red",
#             "random": "green",
#         }
        
#         if (algo.startswith("greedy_tabu")):
#             algo = algo.removeprefix("greedy_tabu").removeprefix("_")
            
#             aspiration: str = ""
#             if (algo.startswith("aspiration")):
#                 algo = algo.removeprefix("aspiration").removeprefix("_")
#                 aspiration = " aspiration"
            
#             push_translations = {
#                 "BestToWorst": "BtW",
#                 "BestToWorstClamped": "BtWc",
#                 "WorstToBest": "WtB",
#                 "Random": "Rand",
#             }
            
#             for push, push_tr in push_translations.items():
#                 if (algo.startswith(push+"_")):
#                     algo = algo.removeprefix(push).removeprefix("_")
                    
#                     tabu_size: str = algo.split("_")[0]
#                     tabu_random: str = algo.split("_")[1].removeprefix("r")
#                     return {
#                         "label": f"GJ tabu{aspiration} {push_tr} {cls.translate_amount(tabu_size)}~{cls.translate_amount(tabu_random)}",
#                         "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
#                         "color": "cyan",
#                     }
#             print(f"malformed algo file label : {full_algo} !")
#             exit(0)
        
#         if (algo.startswith("greedy_lambda")):
#             algo = algo.removeprefix("greedy_lambda").removeprefix("_")
            
#             aspiration: str = ""
#             if (algo.startswith("aspiration")):
#                 algo = algo.removeprefix("aspiration").removeprefix("_")
#                 aspiration = " aspiration"
            
#             return {
#                 "label": f"GJ lambda{aspiration} {cls.translate_amount(algo)}",
#                 "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
#                 "color": "yellow",
#             }
        
#         if (algo.startswith("tabu")):
#             algo = algo.removeprefix("tabu").removeprefix("_")
            
#             aspiration: str = ""
#             if (algo.startswith("aspiration")):
#                 algo = algo.removeprefix("aspiration").removeprefix("_")
#                 aspiration = " aspiration"
                    
#             tabu_size: str = algo.split("_")[0]
#             tabu_random: str = algo.split("_")[1].removeprefix("r")
#             return {
#                 "label": f"tabu{aspiration} {cls.translate_amount(tabu_size)}~{cls.translate_amount(tabu_random)}",
#                 "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
#                 "color": "gray",
#             }
        
#         if (algo.startswith("lambda")):
#             algo = algo.removeprefix("lambda").removeprefix("_")
            
#             aspiration: str = ""
#             if (algo.startswith("aspiration")):
#                 algo = algo.removeprefix("aspiration").removeprefix("_")
#                 aspiration = " aspiration"
            
#             return {
#                 "label": f"lambda{aspiration} {cls.translate_amount(algo)}",
#                 "linestyle": (0, (3, 1, 1, 1, 1, 1)), # densely dashdotdotted (double dot)
#                 "color": "gray",
#             }
        
#         if (algo.startswith("greedy")):
#             algo = algo.removeprefix("greedy").removeprefix("_")
#             greedy_scope_translations = {
#                 "all": "full",
#                 "improve": "improve",
#                 "fixed": "top",
#             }
#             greedy_criterion_translations = {
#                 "best": "best",
#                 "first": "maximal",
#                 "least": "least",
#                 "random": "first",
#             }
            
#             for scope, scope_tr in greedy_scope_translations.items():
#                 if (algo.startswith(scope)):
#                     algo = algo.removeprefix(scope).removeprefix("_")
                    
#                     for criterion, criterion_tr in greedy_criterion_translations.items():
#                         if (algo.startswith(criterion)):
#                             algo = algo.removeprefix(criterion).removeprefix("_")
                            
#                             if (scope == "fixed"):
#                                 return {
#                                         "label": f"GJ {scope_tr}-{cls.translate_amount(algo)} {criterion_tr}",
#                                         "linestyle": (5, (10, 3)), # long dash with offset
#                                         "color": reference_color[criterion],
#                                     }
#                             else:
#                                 return {
#                                         "label": f"GJ {scope_tr} {criterion_tr}",
#                                         "linestyle": "solid" if scope == "all" else "dashdot",
#                                         "color": reference_color[criterion],
#                                     }

#             print(f"malformed algo file label : {full_algo} !")
#             exit(0)
        
#         if (algo.startswith("hc")):
#             algo = algo.removeprefix("hc").removeprefix("_")
#             hc_criterion_translations = {
#                 "best": "best",
#                 "least": "least",
#                 "random": "first",
#                 "first": "first(index based)",
#                 "cycle": "cycle",
#             }
        
#             for criterion, criterion_tr in hc_criterion_translations.items():
#                 if (algo.startswith(criterion)):
#                     algo = algo.removeprefix(criterion).removeprefix("_")
#                     return {
#                             "label": f"HC {criterion_tr}",
#                             "linestyle": "dashed",
#                             "color": reference_color.get(criterion, "grey"),
#                         }
                    
#             print(f"malformed algo file label : {full_algo} !")
#             exit(0)
        
#         print(f"algo file label not recognized : {full_algo} !")
#         exit(0)
    
#     def __init__(self, n: int, k: int, algo: str, instance: str, name: str, path: str):
#         self.name = name
#         self.algo = algo
#         self.N = n
#         self.K = k
#         self.instance = instance
#         self.path = path
        
#         label_and_style = self.get_label_and_style(algo)
#         self.label = label_and_style["label"]
#         self.linestyle = label_and_style["linestyle"]
#         self.color = label_and_style["color"]
#     @classmethod
#     def from_file(cls, path: str):
#         name = os.path.basename(path)
        
#         temp = name.removesuffix(".rundata").split("_")
#         algo, instance = "_".join(temp[:-1]), int(temp[-1])
#         dir: str = os.path.dirname(path)
#         k = int(os.path.basename(dir).removeprefix("K"))
#         dir = os.path.dirname(dir)
#         n = int(os.path.basename(dir).removeprefix("N"))
        
#         result = cls(n, k, algo, instance, name, path)
#         result._data = None
#         return result
#     def get_data(self):
#         if (self._data is not None):
#             return self._data

#         with open(self.path) as f: self._data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
#         self._data.fitness_before_jump /= self.N
#         self._data.fitness_after_jump /= self.N
#         return self._data

#     def __repr__(self)-> str:
#         return self.algo + f" {self.N} {self.K}"

class RunFile:
    _anytime_scores: pd.DataFrame
    _one_run_scores: pd.DataFrame
    _jumps: dict[str, np.ndarray[float]]
    
    _avg_run_score: float
    _avg_run_budget: float
    
    path: str
    algo: str
    
    _label: str
    _linestyle: str
    _color: str
    
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
                "adaptative": "top 2n",
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
    
    def __init__(self, algo: str, path: str):
        self.algo = algo
        self.path = path
        self._anytime_scores = None
        self._one_run_scores = None
        self._avg_run_score = None
        self._avg_run_budget = None
        self._jumps = {}
        
        label_and_style = self.get_label_and_style(algo)
        self.label = label_and_style["label"]
        self.linestyle = label_and_style["linestyle"]
        self.color = label_and_style["color"]
    
    def get_label(self) -> str:
        return self._label
    def get_linestyle(self) -> str:
        return self._linestyle
    def get_color(self) -> str:
        return self._color
    
    def get_similar_data(self) -> list[pd.DataFrame]:
        base_dir: str = os.path.dirname(self.path)

        files: list[str] = [f for f in os.listdir(base_dir) if re.match(r"^_[0-9]+\.rundata$", f.removeprefix(self.algo)) and os.path.isfile(os.path.join(base_dir, f))]
        
        datas: list[pd.DataFrame] = []
        for file in files:
            with open(base_dir + "/" + file) as f:
                data = pd.read_csv(io.StringIO(f.read()), sep = "\t")
                data = data[data.budget <= 1_000_000]
                self.normalize(data)
                datas.append(data)
        
        return datas
    def normalize(self, data: pd.DataFrame):
        print("Normalize not implemented")
        exit(1)
    
    def load_anytime_scores(self):
        datas: list[pd.DataFrame] = self.get_similar_data()
        
        avg_scores: list[list[float]] = []
        avg_budgets: list[list[float]] = []
        
        budget_points: np.ndarray[int] = np.array([])
        fitness_points: np.ndarray[float] = np.array([])
        
        for data in datas:
            budget_points = np.unique(np.concatenate((budget_points, data.budget)))
            budget_points.sort(kind='mergesort')
        
        for data in datas:
            temp: np.ndarray[float] = np.array([data.budget, data.fitness_after_jump])
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
            
            ends = data[(data.size_of_the_jump == 0) * (data.in_run_budget != 1) * (data.budget != data.budget.max())]
            avg_budgets.append([b for b in ends.in_run_budget])
            avg_scores.append([f for f in ends.fitness_after_jump])
        
        avg_max_run_count = min([len(l) for l in avg_budgets])
        self._avg_run_budget = 0
        self._avg_run_score = 0
        for i in range(len(avg_scores)):
            self._avg_run_budget += sum(avg_budgets[i][:avg_max_run_count])
            self._avg_run_score += sum(avg_scores[i][:avg_max_run_count])
        self._avg_run_budget /= len(avg_scores) * avg_max_run_count
        self._avg_run_score /= len(avg_scores) * avg_max_run_count
        self._anytime_scores = pd.DataFrame({"budget": budget_points, "fitness": fitness_points.mean(axis=0)})
    def get_anytime_scores(self) -> pd.DataFrame:
        if (self._anytime_scores is None):
            self.load_anytime_scores()
        return self._anytime_scores

    def split_runs(self, df: pd.DataFrame) -> list[pd.DataFrame]:
        """
        Split concatenated runs assuming each run starts when budget == 1.

        Expected columns:
            - 'budget'
            - 'fitness' (optional for splitting)

        Returns:
            A list of DataFrames, one per run.
        """

        df = df.copy().reset_index(drop=True)

        # Mark the start of each run
        df["new_run"] = df["in_run_budget"] == 1

        # Create run IDs (0, 1, 2, ...)
        df["run_id"] = df["new_run"].cumsum()

        # Split into runs
        runs: list[pd.DataFrame] = [
            group.reset_index(drop=True)
            for _, group in df.groupby("run_id")
        ]

        return runs
    def load_one_run_scores(self):
        datas: list[pd.DataFrame] = self.get_similar_data()
        
        budget_points: np.ndarray[int] = np.array([])
        fitness_points: np.ndarray[float] = np.array([])
        
        for data in datas:
            budget_points = np.unique(np.concatenate((budget_points, data.in_run_budget)))
        budget_points.sort()
        
        for data in datas:
            data = data[data.budget == data.in_run_budget]
            
            temp: np.ndarray[float] = np.array([data.in_run_budget, data.fitness_after_jump])
            one_run_fitness: np.ndarray[float] = np.maximum.accumulate(temp, axis=1)
            
            idx: np.ndarray[int] = np.searchsorted(one_run_fitness[0,:], budget_points, side="right") - 1
            mask: np.ndarray[bool] = idx < 0
            idx: np.ndarray[int] = np.clip(idx, 0, len(one_run_fitness[0,:]) - 1)
            values: np.ndarray[float] = one_run_fitness[1,idx]
            values[mask] = 0
            
            if (fitness_points.shape[0] == 0):
                fitness_points = values
            else:
                fitness_points = np.vstack((fitness_points, values))
        
        self._one_run_scores = pd.DataFrame({"budget": budget_points, "fitness": fitness_points.mean(axis=0)})
    def get_one_run_scores(self) -> pd.DataFrame:
        if (self._one_run_scores is None):
            self.load_one_run_scores()
        return self._one_run_scores

    def load_jumps(self, key: str):
        datas: list[pd.DataFrame] = self.get_similar_data()
        values: np.ndarray[float] = np.array([])
        
        for data in datas:
            values = np.concatenate((values, data[data.size_of_the_jump != 0][key].to_numpy(copy=True)), axis=None)
        
        self._jumps[key] = values
    def get_jumps(self, key: str) -> pd.DataFrame:
        if (key not in self._jumps):
            self.load_jumps(key)
        return self._jumps[key]

    def get_avg_run_budget(self) -> float:
        if (self._avg_run_budget is None):
            self.load_anytime_scores()
        return self._avg_run_budget
    def get_avg_run_score(self) -> float:
        if (self._avg_run_score is None):
            self.load_anytime_scores()
        return self._avg_run_score

    def __repr__(self)-> str:
        return self.algo + " unknown"
class NKRunFile(RunFile):
    N: int
    K: int
    
    def __init__(self, n: int, k: int, algo: str, path: str):
        super().__init__(algo, path)
        self.N = n
        self.K = k
        
    @classmethod
    def from_file(cls, path: str):
        name = os.path.basename(path)
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        k = int(os.path.basename(dir).removeprefix("K"))
        dir = os.path.dirname(dir)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        return cls(n, k, algo, path)

    def normalize(self, data: pd.DataFrame):
        data.fitness_before_jump /= self.N
        data.fitness_after_jump /= self.N
    
    def __repr__(self)-> str:
        return self.algo + f" {self.N} {self.K}"

class NKDataLoader:
    file_infos: dict[int, dict[int, dict[str, NKRunFile]]]
    N_keys: list[str]
    K_keys: list[str]
    Algo_keys: list[str]
    
    def __init__(self, rundata_path: str, small_load: bool = False):
        self.file_infos = {}
        from tqdm import tqdm
        files: list[tuple[str, str]] = []
        for d in os.walk(rundata_path+"/local_search/NK"):
            if (small_load):
                files += [(d[0], f) for f in d[2] if f.endswith("_1.rundata") and "N50/K0" in d[0]]
            else:
                files += [(d[0], f) for f in d[2] if f.endswith("_1.rundata")]

        for i in tqdm(range(len(files))):
            d, f = files[i]
            info: NKRunFile = NKRunFile.from_file(d + "/" + f)
            if (info != None):
                self.file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo, info)

        self.N_keys = sorted(self.file_infos.keys())
        self.K_keys = sorted(self.file_infos[self.N_keys[0]].keys())
        self.Algo_keys = sorted(self.file_infos[self.N_keys[0]][self.K_keys[0]].keys())
    
    def get_file(self, n: int, k: int, algo: str) -> RunFile:
        return self.file_infos[n][k][algo]
    def get_reference_file(self, algo: str) -> RunFile:
        return self.get_file(self.N_keys[0], self.K_keys[0], algo)