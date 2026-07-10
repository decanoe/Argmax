from concurrent.futures import ThreadPoolExecutor, as_completed

from tqdm import tqdm
from algo_labels import Algo
from data_loader import DataLoader

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
                and ("adaptative_n+5" not in algo or "least" in algo or "first" in algo or "best" in algo)
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
                if (problem in data_loaders):
                    futures.append(executor.submit(create_csv, data_loaders[problem], algo.algo, output_path + f"/{problem}/" + algo.get_full_label(lang = 'plot', **kwargs).replace(" ", "_") + ".csv"))

        for future in tqdm(as_completed(futures), total=len(futures)):
            future.result()

def generate_all_iterated_csv(data_loaders: dict[str, DataLoader], output_path: str, **kwargs):
    output_path += "/iterated"
    
    def hc_selector(algo: str) -> bool:
        return (algo.startswith("hc_")
                and "cycle" not in algo
                and "first" not in algo
        ) or   (algo.startswith("tabu_aspiration_.1_r0")
        )
    def gj_selector(algo: str) -> bool:
        return (algo.startswith("greedy_")
                and "fixed" not in algo
                and "adaptative_max(2n,5)" not in algo
                and ("adaptative_n+5" not in algo or "least" in algo or "first" in algo or "best" in algo)
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
                if (problem in data_loaders):
                    futures.append(executor.submit(create_csv, data_loaders[problem], algo.algo, output_path + f"/{problem}/" + algo.get_full_label(lang = 'plot', **kwargs).replace(" ", "_") + ".csv"))

        for future in tqdm(as_completed(futures), total=len(futures)):
            future.result()
