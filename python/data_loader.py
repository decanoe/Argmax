from __future__ import annotations
import os
import re
from typing import Any
import pandas as pd
import numpy as np
import io
from algo_labels import Algo

class RunFile:
    _anytime_scores: pd.DataFrame
    _one_run_scores: pd.DataFrame
    _jumps: dict[str, np.ndarray[float]]
    
    _avg_run_score: float
    _avg_run_budget: float
    
    path: str
    algo_infos: Algo
    
    @classmethod
    def translate_amount(cls, amount: str):
        try:
            v = float(amount)
            return f"{int(v * 100)}%"
        except ValueError:
            print(f"can't translate amount for {amount} !")
            exit(0)
    
    def __init__(self, algo: str, path: str):
        self.algo_infos = Algo.from_algo(algo)
        self.path = path
        self._anytime_scores = None
        self._one_run_scores = None
        self._avg_run_score = None
        self._avg_run_budget = None
        self._jumps = {}
    
    def get_label(self) -> str:
        return self.algo_infos.get_plot_label()
    def get_linestyle(self) -> str:
        return self.algo_infos.get_plot_style()
    def get_color(self) -> str:
        return self.algo_infos.get_plot_color()
    
    def get_similar_data(self) -> list[pd.DataFrame]:
        base_dir: str = os.path.dirname(self.path)

        files: list[str] = [f for f in os.listdir(base_dir) if re.match(r"^_[0-9]+\.rundata$", f.removeprefix(self.algo_infos.algo)) and os.path.isfile(os.path.join(base_dir, f))]
        
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
    
    def load_anytime_scores(self, datas: list[pd.DataFrame] = None):
        if (self._anytime_scores is not None): return
        if (datas is None): datas = self.get_similar_data()
        self.load_avg_values(datas)
        
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
        
        fitness_points = fitness_points.mean(axis = 0)
        mask = np.ones(len(fitness_points), dtype=bool)
        mask[1:-1] = ~(
            (fitness_points[1:-1] == fitness_points[:-2]) &
            (fitness_points[1:-1] == fitness_points[2:])
        )
        
        self._anytime_scores = pd.DataFrame({"budget": budget_points[mask], "fitness": fitness_points[mask]})
    def get_anytime_scores(self, budget: int = 1_000_000) -> pd.DataFrame:
        if (self._anytime_scores is None):
            self.load_anytime_scores()
        return self._anytime_scores[self._anytime_scores.budget <= budget]

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
    def load_one_run_scores(self, datas: list[pd.DataFrame] = None):
        if (self._one_run_scores is not None): return
        if (datas is None): datas = self.get_similar_data()
        
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
        return self._jumps[key].copy()

    def load_avg_values(self, datas: list[pd.DataFrame] = None):
        if (self._avg_run_score is not None and self._avg_run_budget is not None): return
        if (datas is None): datas = self.get_similar_data()
        
        avg_scores: list[list[float]] = []
        avg_budgets: list[list[float]] = []
        
        for data in datas:
            ends = data[(data.size_of_the_jump == 0) * (data.in_run_budget != 1) * (data.budget != data.budget.max())]
            avg_budgets.append([b for b in ends.in_run_budget])
            avg_scores.append([f for f in ends.fitness_after_jump])
        
        avg_max_run_count = max(min([len(l) for l in avg_budgets]), 1)
        self._avg_run_budget = 0
        self._avg_run_score = 0
        for i in range(len(avg_scores)):
            self._avg_run_budget += sum(avg_budgets[i][:avg_max_run_count])
            self._avg_run_score += sum(avg_scores[i][:avg_max_run_count])
        self._avg_run_budget /= len(avg_scores) * avg_max_run_count
        self._avg_run_score /= len(avg_scores) * avg_max_run_count
    def get_avg_run_budget(self) -> float:
        if (self._avg_run_budget is None):
            self.load_avg_values()
        return self._avg_run_budget
    def get_avg_run_score(self) -> float:
        if (self._avg_run_score is None):
            self.load_avg_values()
        return self._avg_run_score

    def __repr__(self)-> str:
        return self.algo_infos + " unknown"
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
        return self.algo_infos + f" {self.N} {self.K}"
class QuboRunFile(RunFile):
    N: int
    
    def __init__(self, n: int, algo: str, path: str):
        super().__init__(algo, path)
        self.N = n
        
    @classmethod
    def from_file(cls, path: str):
        name = os.path.basename(path)
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        return cls(n, algo, path)

    def normalize(self, data: pd.DataFrame):
        pass
    
    def __repr__(self)-> str:
        return self.algo_infos + f" Qubo {self.N}"
class SatRunFile(RunFile):
    N: int
    type_name: str
    
    def __init__(self, n: int, type_name: str, algo: str, path: str):
        super().__init__(algo, path)
        self.N = n
        self.type_name = type_name
        
    @classmethod
    def from_file(cls, path: str):
        name = os.path.basename(path)
        
        temp = name.removesuffix(".rundata").split("_")
        algo, instance = "_".join(temp[:-1]), int(temp[-1])
        dir: str = os.path.dirname(path)
        type_name = os.path.basename(dir)
        dir = os.path.dirname(dir)
        n = int(os.path.basename(dir).removeprefix("N"))
        
        return cls(n, type_name, algo, path)

    def normalize(self, data: pd.DataFrame):
        pass
    
    def __repr__(self)-> str:
        return self.algo_infos + f" Sat {self.type_name} {self.N}"

class DataLoader:
    Algo_keys: list[str]
    
    def load_file(self, file: str, dir: str):
        pass
    def __init__(self, dir_path: str):
        from tqdm import tqdm
        files: list[tuple[str, str]] = []
        for d in os.walk(dir_path):
            files += [(d[0], f) for f in d[2] if f.endswith("_1.rundata")]

        for i in tqdm(range(len(files))):
            d, f = files[i]
            self.load_file(f, d)
    
    def get_file(self, algo: str) -> RunFile:
        pass
    def set_parameters(self, **kwargs) -> DataLoader:
        return self

    def get_parameters_iterator(self) -> list[tuple[str, dict[str, Any]]]:
        return []
class NKDataLoader(DataLoader):
    file_infos: dict[int, dict[int, dict[str, NKRunFile]]]
    N_keys: list[str]
    K_keys: list[str]
    n: int
    k: int
    
    def load_file(self, file: str, dir: str):
        info: NKRunFile = NKRunFile.from_file(dir + "/" + file)
        if (info != None):
            self.file_infos.setdefault(info.N, {}).setdefault(info.K, {}).setdefault(info.algo_infos.algo, info)
    def __init__(self, rundata_path: str):
        self.file_infos = {}
        super().__init__(rundata_path+"/NK")

        self.N_keys = sorted(self.file_infos.keys())
        self.K_keys = sorted(self.file_infos[self.N_keys[0]].keys())
        self.Algo_keys = sorted(self.file_infos[self.N_keys[0]][self.K_keys[0]].keys())
        
        self.n = self.N_keys[0]
        self.k = self.K_keys[0]
    
    def get_file(self, algo: str) -> RunFile:
        return self.file_infos[self.n][self.k][algo]
    def set_parameters(self, **kwargs):
        if (kwargs.get("n", self.n) in self.N_keys): self.n = kwargs.get("n")
        if (kwargs.get("k", self.k) in self.K_keys): self.k = kwargs.get("k")
        return super().set_parameters(**kwargs)
    
    def get_parameters_iterator(self) -> list[tuple[str, dict[str, Any]]]:
        result: list[tuple[str, dict[str, int]]] = []
        for n, n_data in sorted(self.file_infos.items(), key=lambda p : p[0]):
            for k, _ in sorted(n_data.items(), key=lambda p : p[0]):
                result.append((f"N{n}_K{k}", {"n":n, "k":k}))
        return result
class QuboDataLoader(DataLoader):
    file_infos: dict[int, dict[str, NKRunFile]]
    N_keys: list[str]
    n: int
    
    def load_file(self, file: str, dir: str):
        info: QuboRunFile = QuboRunFile.from_file(dir + "/" + file)
        if (info != None):
            self.file_infos.setdefault(info.N, {}).setdefault(info.algo_infos.algo, info)
    def __init__(self, rundata_path: str):
        self.file_infos = {}
        super().__init__(rundata_path+"/Qubo")

        self.N_keys = sorted(self.file_infos.keys())
        self.Algo_keys = sorted(self.file_infos[self.N_keys[0]].keys())
        
        self.n = self.N_keys[0]
    
    def get_file(self, algo: str) -> RunFile:
        return self.file_infos[self.n][algo]
    def set_parameters(self, **kwargs):
        if (kwargs.get("n", self.n) in self.N_keys): self.n = kwargs.get("n")
        return super().set_parameters(**kwargs)
    
    def get_parameters_iterator(self) -> list[tuple[str, dict[str, Any]]]:
        result: list[tuple[str, dict[str, int]]] = []
        for n, _ in sorted(self.file_infos.items(), key=lambda p : p[0]):
            result.append((f"N{n}_Qubo", {"n":n}))
        return result
class SatDataLoader(DataLoader):
    file_infos: dict[int, dict[str, dict[str, NKRunFile]]]
    N_keys: list[str]
    type_keys: list[str]
    n: int
    type_name: str
    
    def load_file(self, file: str, dir: str):
        info: SatRunFile = SatRunFile.from_file(dir + "/" + file)
        if (info != None):
            self.file_infos.setdefault(info.N, {}).setdefault(info.type_name, {}).setdefault(info.algo_infos.algo, info)
    def __init__(self, rundata_path: str):
        self.file_infos = {}
        super().__init__(rundata_path+"/Sat")

        self.N_keys = sorted(self.file_infos.keys())
        self.type_keys = sorted(self.file_infos[self.N_keys[0]].keys())
        self.Algo_keys = sorted(self.file_infos[self.N_keys[0]][self.type_keys[0]].keys())
        
        self.n = self.N_keys[0]
        self.type_name = self.type_keys[1]
    
    def get_file(self, algo: str) -> RunFile:
        return self.file_infos[self.n][self.type_name][algo]
    def set_parameters(self, **kwargs):
        if (kwargs.get("n", self.n) in self.N_keys): self.n = kwargs.get("n")
        if (kwargs.get("type_name", self.type_name) in self.type_keys): self.type_name = kwargs.get("type_name")
        return super().set_parameters(**kwargs)
    
    def get_parameters_iterator(self) -> list[tuple[str, dict[str, Any]]]:
        result: list[tuple[str, dict[str, Any]]] = []
        for n, n_data in sorted(self.file_infos.items(), key=lambda p : p[0]):
            for t, _ in sorted(n_data.items(), key=lambda p : p[0]):
                result.append((f"N{n}_Sat_{t}", {"n":n, "type_name":t}))
        return result
