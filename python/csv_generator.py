from concurrent.futures import ThreadPoolExecutor, as_completed
import os
from typing import Any, Literal

import pandas as pd
from tqdm import tqdm
from algo_labels import Algo
from data_loader import DataLoader, NKDataLoader, QuboDataLoader, RunFile, SatDataLoader

def generate_all_non_iterated_csv(data_loaders: dict[str, DataLoader], output_path: str, **kwargs):
    output_path += "/non_iterated"
    
    def hc_selector(algo: str) -> bool:
        return (algo.startswith("hc_")
                and "cycle" not in algo
                and "first" not in algo
        )
    def gj_selector(algo: str) -> bool:
        return (algo.startswith("greedy_")
                and "fixed" not in algo
                and "adaptative_max(2n,5)" not in algo
                and ("adaptative_n+5" not in algo or "least" in algo or "first" in algo)
                and "guided" not in algo
                and "tabu" not in algo
                and "lambda" not in algo
                and "Asc+" not in algo and "Asc-" not in algo
                and "Rand+" not in algo and "Rand-" not in algo
                
                and "median" not in algo
                and "middle" not in algo
                and "random" not in algo
        )
    
    hc_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if hc_selector(algo)], key=lambda t: t.get_algo())
    gj_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if gj_selector(algo)], key=lambda t: t.get_algo())
    algo_list: list[Algo] = hc_algos + gj_algos
    print(f"{len(algo_list)} algos in tables")
    print([algo.get_full_label(lang='plot', **kwargs) for algo in algo_list])
    
    def create_csv(data_loader: DataLoader, algo: str, file: str):
        data_loader.to_csv(algo, file, False)
    
    futures = []
    with ThreadPoolExecutor(max_workers=8) as executor:
        for algo in algo_list:
            for problem in ["NK", "Sat", "Qubo"]:
                futures.append(executor.submit(create_csv, data_loaders[problem], algo.algo, output_path + f"/{problem}/" + algo.get_full_label(lang = 'plot', **kwargs).replace(" ", "_") + ".csv"))

        for future in tqdm(as_completed(futures), total=len(futures)):
            future.result()

def generate_all_iterated_csv(data_loaders: dict[str, DataLoader], output_path: str, **kwargs):
    output_path += "/iterated"
    
    def hc_selector(algo: str) -> bool:
        return (algo.startswith("hc_")
                and "cycle" not in algo
                and "first" not in algo
        )
    def gj_selector(algo: str) -> bool:
        return (algo.startswith("greedy_")
                and "fixed" not in algo
                and "adaptative_max(2n,5)" not in algo
                and ("adaptative_n+5" not in algo or "least" in algo or "first" in algo)
                and "guided" not in algo
                and "tabu" not in algo
                and "lambda" not in algo
                and "Asc+" not in algo and "Asc-" not in algo
                and "Rand+" not in algo and "Rand-" not in algo
                
                and "median" not in algo
                and "middle" not in algo
                and "random" not in algo
        )
    
    hc_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if hc_selector(algo)], key=lambda t: t.get_algo())
    gj_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if gj_selector(algo)], key=lambda t: t.get_algo())
    algo_list: list[Algo] = hc_algos + gj_algos
    print(f"{len(algo_list)} algos in tables")
    print([algo.get_full_label(lang='plot', **kwargs) for algo in algo_list])
    
    def create_csv(data_loader: DataLoader, algo: str, file: str):
        data_loader.to_csv(algo, file, True)
    
    futures = []
    with ThreadPoolExecutor(max_workers=8) as executor:
        for algo in algo_list:
            for problem in ["NK", "Sat", "Qubo"]:
                futures.append(executor.submit(create_csv, data_loaders[problem], algo.algo, output_path + f"/{problem}/" + algo.get_full_label(lang = 'plot', **kwargs).replace(" ", "_") + ".csv"))

        for future in tqdm(as_completed(futures), total=len(futures)):
            future.result()

def special_to_csv_lines(file: RunFile, numrun: int) -> str:
    result: str = ""
    
    datas = file.get_similar_data()
    for i in range(len(datas)):
        data: pd.DataFrame = datas[i]
        
        for budget in [10_000, 100_000, 1_000_000]:
            result += file.to_csv_line_iterated(i, numrun, budget, data[data.budget <= budget].fitness_after_jump.max(), "numrun=seeds")
        
    return result
def special_cases(dir_path: str, output_path, **kwargs):
    output_path += "/iterated"
    true_algo_name : str = "tabu_aspiration_.1_r0"
    
    headers: dict[str, str] = {}
    bodies: dict[str, str] = {}
    
    for i in range(10):
        directory: str = dir_path+f"/../rundata/local_search/tabu_1_000_000_run{i}"
        data_loaders: dict[str, DataLoader] = {
            "NK": NKDataLoader(directory, **kwargs),
            "Sat": SatDataLoader(directory, **kwargs),
            "Qubo": QuboDataLoader(directory, **kwargs),
        }
        
        for problem in ["NK", "Sat", "Qubo"]:
            headers.setdefault(problem, None)
            bodies.setdefault(problem, "")
            for _, keys in data_loaders[problem].get_parameters_iterator():
                file: RunFile = data_loaders[problem].get_with_parameters(true_algo_name, **keys)
                if (file is None): continue
                
                if (headers[problem] is None): headers[problem] = file.csv_header(True)
                elif(headers[problem] != file.csv_header(True)): raise AssertionError().add_note(f'headers do not match : "{headers[problem]}" != "{file.csv_header(True)}"')
                
                bodies[problem] += special_to_csv_lines(file, i)
            
    for problem in ["NK", "Sat", "Qubo"]:
        algo: Algo = data_loaders[problem].get_file(true_algo_name).algo_infos
        save_path = output_path + f"/{problem}/" + algo.get_full_label(lang = 'plot', **kwargs).replace(" ", "_") + ".csv"
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        with open(save_path, 'w') as f: f.write(headers[problem] + bodies[problem])