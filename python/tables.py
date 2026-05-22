import os
from typing import Literal
from algo_labels import Algo
from data_loader import DataLoader, NKDataLoader
import re

def add_spaces(value: float, str_size: int):
    return str(value) + " " * (str_size - len(str(value)))

COUNT_PER_INSTANCE: int = 2 # nb of runs to take per instance (should not exceed the number of runs done on each instances)

class Table:
    corner_header: str
    headers: list[Algo]
    lines_headers: list[str]
    lines: list[list[float]]
    
    highlight: Literal['max', 'min', 'none']
    
    def __init__(self, corner_header: str, headers: list[Algo], highlight: Literal['max', 'min', 'none'] = 'none'):
        self.corner_header = corner_header
        self.headers = headers
        self.lines_headers = []
        self.lines = []
        self.highlight = highlight
    def add_line(self, line_header: str, line: list[float]):
        self.lines_headers.append(line_header)
        self.lines.append(line)
    
    def format_value(self, value: float) -> str:
        return "%01.4f"%value
    def do_highlight(self, value: float, line_values: list[float]) -> bool:
        if (self.highlight == 'max'): return value == max(line_values)
        if (self.highlight == 'min'): return value == min(line_values)
        return False
    
    def get_md_title(self) -> str | None: return None
    def to_md(self) -> str:
        cell_size: int = max([len(f"<ins>{self.format_value(v)}</ins>") for line in self.lines for v in line] + [len(f"**{h}**") for h in [self.corner_header] + [h.get_plot_label() for h in self.headers] + self.lines_headers])
        
        result: str = "| " + " | ".join([add_spaces(f"**{h}**", cell_size) for h in [self.corner_header] + [h.get_plot_label() for h in self.headers]]) + " |\n"
        result += "| " + " | ".join(["-" * cell_size] * (len(self.headers) + 1)) + " |\n"
        
        for header, line in zip(self.lines_headers, self.lines, strict=True):
            modified_line = [f"<ins>{self.format_value(v)}</ins>" if self.do_highlight(v, line) else self.format_value(v) for v in line]
            result += f"| {add_spaces(f"**{header}**", cell_size)} | " + " | ".join([add_spaces(h, cell_size) for h in modified_line]) + " |\n"
        
        if (self.get_md_title() is not None): result = f"# {self.get_md_title()}\n{result}"
        return result
    def save_md(self, path: str):
        with open(path, 'w') as f: f.write(self.to_md())

    def get_tex_title(self) -> str | None: return None
    def get_tex_label(self) -> str | None: return None
    def to_tex(self) -> str:
        # list of tuple (header, category, category_sort_index, old_index)
        modified_headers: list[tuple[str, str, int, int]] = [(self.headers[i].get_tex_name(), self.headers[i].get_tex_category(), self.headers[i].get_tex_category_sort_index(), i) for i in range(len(self.headers))]
        modified_headers = sorted(modified_headers, key=lambda h:(h[2], h[1], h[3]))
        
        cell_size: int = max([len(f"\\underline[{self.format_value(v)}]") for line in self.lines for v in line] + [len(f"\\textbf[{h}]") for h in [self.corner_header] + [h[0] for h in modified_headers] + self.lines_headers])
        
        tex_headers: str = "\\begin{table}[ht]\n"
        if (self.get_tex_title() is not None): tex_headers += "\\caption{" + self.get_tex_title() + "}\n"
        if (self.get_tex_label() is not None): tex_headers += "\\label{" + self.get_tex_label() + "}"
        
        tex_headers += """
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
"""
        # list of (category header, category size, category_sort_index)
        categories: list[tuple[str, int]] = [(modified_headers[0][1], 0)]
        for _, category, _, _ in modified_headers:
            if (category == categories[-1][0]): categories[-1] = (categories[-1][0], categories[-1][1] + 1)
            else: categories.append((category, 1))
        
        tex_headers += "\\begin{tabular}{l |" + " | ".join([" ".join("c" * c[1]) for c in categories]) + "}\n"
        tex_headers += "\\hline\n"
        tex_headers += add_spaces("", cell_size)
        for category, size in categories:
            if size == 1:   tex_headers += f" & {add_spaces(category, cell_size)}"
            else:           tex_headers += " & " + add_spaces("\\multicolumn{" + str(size) + "}{" + ("c|" if category != categories[-1][0] else "c") + "}{" + category + "}", cell_size * size + len(" & ") * (size - 1))
        tex_headers += "\\\\ \n"
        tex_headers += " & ".join([add_spaces(h, cell_size) for h in [self.corner_header] + [h[0] for h in modified_headers]])
        tex_headers += "\\\\ \n"
        tex_headers += "\\hline\n"
        
        tex_footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""

        tex_body: str = ""
        
        current_n: str = "-1"
        for header, line in zip(self.lines_headers, self.lines, strict=True):
            new_n: str = header.split("_")[0]
            if (current_n != new_n):
                tex_body += "\\hline\n"
                current_n = new_n
            
            tex_body += add_spaces("\\textbf{" + header.replace("_", " ") + "}", cell_size)
            
            for _, _, _, index in modified_headers:
                value = line[index]
                if self.do_highlight(value, line):
                    tex_body += " & " + add_spaces("\\underline{" + self.format_value(value) + "}", cell_size)
                else:
                    tex_body += " & " + add_spaces(self.format_value(value), cell_size)
            tex_body += "\\\\\n"
        tex_body += "\\hline\n"
        
        return tex_headers + tex_body + tex_footer
    def save_tex(self, path: str):
        with open(path, 'w') as f: f.write(self.to_tex())

class AvgScoreTable(Table):
    def __init__(self, data_loaders: list[DataLoader], algo_list: list[Algo]):
        super().__init__("Instances", algo_list, 'max')
        
        print("Creating AvgScoreTable")
        for data_loader in data_loaders:
            for instance, keys in data_loader.get_parameters_iterator():
                data_loader.set_parameters(**keys)
                print(f"\t{instance}")
                self.add_line(instance, [round(data_loader.get_file(algo.get_algo()).get_avg_run_score(), 4) for algo in algo_list])
    
    def get_md_title(self):
        return "Mean fitness score per execution. Results are averaged over " + str(COUNT_PER_INSTANCE * 10) + " instances per (N,K) pair. The best results for each instance are underlined."
    
    def get_tex_title(self):
        return "Mean fitness score per execution. Results are averaged over " + str(COUNT_PER_INSTANCE * 10) + " instances per $(N,K)$ pair. The best results for each instance are underlined."
    def get_tex_label(self):
        return "tab:mean_scores"
class MaxScoreTable(Table):
    budget: int
    
    def __init__(self, data_loaders: list[DataLoader], algo_list: list[tuple[str, str]], budget: int):
        super().__init__("Instances", algo_list, 'max')
        self.budget = budget
        
        print(f"Creating MaxScoreTable({budget})")
        for data_loader in data_loaders:
            for instance, keys in data_loader.get_parameters_iterator():
                data_loader.set_parameters(**keys)
                print(f"\t{instance}")
                self.add_line(instance, [round(data_loader.get_file(algo.get_algo()).get_anytime_scores(budget).fitness.max(), 4) for algo in algo_list])
                
    def get_md_title(self):
        if (self.budget == 1_000): return "Best fitness achieved under a limited budget of 1,000 evaluations. Underlined values indicate the best performance for each instance."
        if (self.budget == 1_000_000): return "Best fitness achieved under a large budget of 1,000,000 evaluations. Underlined values indicate the best performance for each instance."
        return f"Best fitness obtained for a budget of {self.budget//1000},000 evaluations."
    
    def get_tex_title(self):
        return self.get_md_title()
    def get_tex_label(self):
        return f"tab:budget{self.budget//1000}k"
class AvgBudgetTable(Table):
    def __init__(self, data_loaders: list[DataLoader], algo_list: list[Algo]):
        super().__init__("Instances", algo_list, 'min')
        
        print("Creating AvgBudgetTable")
        for data_loader in data_loaders:
            for instance, keys in data_loader.get_parameters_iterator():
                data_loader.set_parameters(**keys)
                print(f"\t{instance}")
                self.add_line(instance, [data_loader.get_file(algo.get_algo()).get_avg_run_budget() for algo in algo_list])
    
    def format_value(self, value: float) -> str:
        return "%.1f"%value
    
    def get_md_title(self):
        return "Mean evaluation budget per single trajectory. Data averaged over " + str(COUNT_PER_INSTANCE * 10) + " independent runs per $(N,K)$ pair across 10 problem instances."
        
    def get_tex_title(self):
        return "Mean evaluation budget per single trajectory. Data averaged over " + str(COUNT_PER_INSTANCE * 10) + " independent runs per (N,K) pair across 10 problem instances."
    def get_tex_label(self):
        return "tab:mean_budget"

def generate_all_tables(data_loaders: dict[str, DataLoader], output_path: str, **kwargs):
    def hc_selector(algo: str) -> bool:
        return (algo.startswith("hc_")
                and "cycle" not in algo
                and "first" not in algo
        )
    def gj_selector(algo: str) -> bool:
        return (algo.startswith("greedy_")
                and ("fixed" not in algo or re.search("fixed_[^_]*_\\.25", algo))
                and ("adaptative" not in algo or "least" in algo or "first" in algo)
                and ("guided" not in algo or "guided_best_1" in algo)
                and "tabu" not in algo
                and "lambda" not in algo
                and "Asc+" not in algo and "Asc-" not in algo
                and "Rand+" not in algo and "Rand-" not in algo
        )
    
    hc_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if hc_selector(algo)], key=lambda t: t.get_algo())
    gj_algos: list[Algo] = sorted([data_loaders["NK"].get_file(algo).algo_infos for algo in data_loaders["NK"].Algo_keys if gj_selector(algo)], key=lambda t: t.get_algo())
    algo_list: list[Algo] = hc_algos + gj_algos
    print(f"{len(algo_list)} algos in tables")
    print([algo.get_plot_label() for algo in algo_list])
    
    table_data_loaders: list[DataLoader] = [data_loaders["NK"], data_loaders["Sat"], data_loaders["Qubo"]]
    
    table = AvgScoreTable(table_data_loaders, algo_list)
    table.save_md(output_path + "/avg.md", **kwargs)
    table.save_tex(output_path + "/avg.tex", **kwargs)
    
    table = AvgBudgetTable(table_data_loaders, algo_list)
    table.save_md(output_path + "/budgets.md", **kwargs)
    table.save_tex(output_path + "/budgets.tex", **kwargs)
    
    for budget in [1_000, 10_000, 100_000, 1_000_000]:
        table = MaxScoreTable(table_data_loaders, algo_list, budget)
        table.save_md(output_path + f"/best_{budget:_}.md", **kwargs)
        table.save_tex(output_path + f"/best_{budget:_}.tex", **kwargs)
