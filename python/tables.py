from typing import Literal

from pandas import DataFrame

from data_loader import NKDataLoader

def add_spaces(value: float, str_size: int):
    return str(value) + " " * (str_size - len(str(value)))

COUNT_PER_INSTANCE: int = 2 # nb of runs to take per instance (should not exceed the number of runs done on each instances)

class Table:
    corner_header: str
    headers: list[str]
    lines_headers: list[str]
    lines: list[list[float]]
    
    highlight: Literal['max', 'min', 'none']
    
    def __init__(self, corner_header: str, headers: list[str], highlight: Literal['max', 'min', 'none'] = 'none'):
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
        cell_size: int = max([len(f"<ins>{self.format_value(v)}</ins>") for line in self.lines for v in line] + [len(f"**{h}**") for h in [self.corner_header] + self.headers + self.lines_headers])
        
        result: str = "| " + " | ".join([add_spaces(f"**{h}**", cell_size) for h in [self.corner_header] + self.headers]) + " |\n"
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
        cell_size: int = max([len(f"\\underline[{self.format_value(v)}]") for line in self.lines for v in line] + [len(f"\\textbf[{h}]") for h in [self.corner_header] + self.headers + self.lines_headers])
        
        tex_headers: str = "\\begin{table}[ht]\n"
        if (self.get_tex_title() is not None): tex_headers += "\\caption{" + self.get_tex_title() + "}\n"
        if (self.get_tex_label() is not None): tex_headers += "\\label{" + self.get_tex_label() + "}"
        tex_headers += """
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
"""
        tex_headers += "\\begin{tabular}{l | " + " ".join("c" * len(self.headers)) + "}\n"
        tex_headers += "\\hline\n"
        tex_headers += " & ".join([add_spaces(h.replace("%", "\\%"), cell_size) for h in ["$(N,K)$"] + self.headers]) + "\\\\\n"
        
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
            
            tex_body += add_spaces("\\textbf{" + header.replace("_", ", ") + "} ", cell_size)
            
            for value in line:
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
    def __init__(self, data_loader: NKDataLoader, algo_list: list[tuple[str, str]]):
        super().__init__("instance (N_K)", [algo[0] for algo in algo_list], 'max')
        
        for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
            for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
                for algo_header, algo in algo_list:
                    print(algo, ":", round(k_data[algo].get_avg_run_score(), 4))
                exit(0)
                self.add_line(f"{n}_{k}", [k_data[algo].get_avg_run_score() for algo_header, algo in algo_list])
    
    def get_md_title(self):
        return "Mean fitness score per execution. Results are averaged over " + str(COUNT_PER_INSTANCE * 10) + " instances per (N,K) pair. The best results for each instance are underlined."
    
    def get_tex_title(self):
        return "Mean fitness score per execution. Results are averaged over " + str(COUNT_PER_INSTANCE * 10) + " instances per $(N,K)$ pair. The best results for each instance are underlined."
    def get_tex_label(self):
        return "tab:mean_scores"
class MaxScoreTable(Table):
    budget: int
    
    def __init__(self, data_loader: NKDataLoader, algo_list: list[tuple[str, str]], budget: int):
        super().__init__("instance (N_K)", [algo[0] for algo in algo_list], 'max')
        self.budget = budget
        
        for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
            for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
                self.add_line(f"{n}_{k}", [k_data[algo].get_anytime_scores().fitness.max() for algo_header, algo in algo_list])
                
    def get_md_title(self):
        if (self.budget == 1_000): return "Best fitness achieved under a limited budget of 1,000 evaluations. Underlined values indicate the best performance for each instance."
        if (self.budget == 100_000): return "Best fitness achieved under a large budget of 100,000 evaluations. Underlined values indicate the best performance for each instance."
        return f"Best fitness obtained for a budget of {self.budget//1000},000 evaluations."
    
    def get_tex_title(self):
        return self.get_md_title()
    def get_tex_label(self):
        return f"tab:budget{self.budget//1000}k"
class AvgBudgetTable(Table):
    def __init__(self, data_loader: NKDataLoader, algo_list: list[tuple[str, str]]):
        super().__init__("instance (N_K)", [algo[0] for algo in algo_list], 'min')
        
        for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
            for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
                self.add_line(f"{n}_{k}", [k_data[algo].get_avg_run_budget() for algo_header, algo in algo_list])
    
    def format_value(self, value: float) -> str:
        return "%.1f"%value
    
    def get_md_title(self):
        return "Mean evaluation budget per single trajectory. Data averaged over " + str(COUNT_PER_INSTANCE * 10) + " independent runs per $(N,K)$ pair across 10 problem instances."
        
    def get_tex_title(self):
        return "Mean evaluation budget per single trajectory. Data averaged over " + str(COUNT_PER_INSTANCE * 10) + " independent runs per (N,K) pair across 10 problem instances."
    def get_tex_label(self):
        return "tab:mean_budget"

def generate_all_tables(data_loader: NKDataLoader, output_path: str):
    hc_selector = lambda algo: algo.startswith("hc_") and "cycle" not in algo and "first" not in algo
    gj_selector = lambda algo: algo.startswith("greedy_") and ("fixed" not in algo or ".5" in algo) and "tabu" not in algo and "lambda" not in algo
    
    hc_algos: list[tuple[str, str]] = sorted([(data_loader.get_reference_file(algo).label, algo) for algo in data_loader.Algo_keys if hc_selector(algo)], key=lambda t: t[0])
    gj_algos: list[tuple[str, str]] = sorted([(data_loader.get_reference_file(algo).label, algo) for algo in data_loader.Algo_keys if gj_selector(algo)], key=lambda t: t[0])
    algo_list: list[str] = hc_algos + gj_algos
    
    table = AvgScoreTable(data_loader, algo_list)
    table.save_md(output_path + "/avg.md")
    table.save_tex(output_path + "/avg.tex")
    
    table = AvgBudgetTable(data_loader, algo_list)
    table.save_md(output_path + "/budgets.md")
    table.save_tex(output_path + "/budgets.tex")
    
    budgets = [1_000, 10_000, 100_000]
    
    for i in range(len(budgets)):
        table = MaxScoreTable(data_loader, algo_list, budgets[i])
        table.save_md(output_path + f"/best_{budgets[i]//1000}_000.md")
        table.save_tex(output_path + f"/best_{budgets[i]//1000}_000.tex")
