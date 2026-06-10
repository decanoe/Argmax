import os
import threading
from typing import Any, Literal

import tqdm
from algo_labels import Algo
from data_loader import DataLoader, NKDataLoader, RunFile
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
    
    def do_highlight(self, value: float, line_values: list[float]) -> bool:
        if (self.highlight == 'max'): return value == max(line_values)
        if (self.highlight == 'min'): return value == min(line_values)
        return False
    
    def get_md_title(self) -> str | None: return None
    def get_tex_title(self) -> str | None: return None
    def get_tex_label(self) -> str | None: return None
    
class TableSaver:
    table: Table
    lang: Literal['md', 'tex']
    kwargs: dict[str, Any]
    
    def __init__(self, table: Table, lang: Literal['md', 'tex'], **kwargs):
        self.table = table
        self.lang = lang
        self.kwargs = kwargs
    
    def get_categories(self, modified_headers: list[tuple[str, str, int, int]] = None) -> list[str, int]:
        ''' list of tuple (category header, category size) '''
        if modified_headers is None: modified_headers = self.get_modified_headers()
        
        categories: list[tuple[str, int]] = [(modified_headers[0][1], 0)]
        for _, category, _, _ in modified_headers:
            if (category == categories[-1][0]): categories[-1] = (categories[-1][0], categories[-1][1] + 1)
            else: categories.append((category, 1))
        return categories
    def get_modified_headers(self) -> list[tuple[str, str, int, int]]:
        ''' list of tuple (header, category, category_sort_index, old_index) '''
        headers: list[Algo] = self.table.headers
        modified_headers: list[tuple[str, str, int, int]] = [(headers[i].get_label(self.lang, **self.kwargs), headers[i].get_category(self.lang, **self.kwargs), headers[i].get_category_sort_index(), i) for i in range(len(headers))]
        modified_headers = sorted(modified_headers, key=lambda h:(h[2], h[1], h[3]))
        return modified_headers
    def get_cell_size(self, modified_headers: list[tuple[str, str, int, int]] = None) -> int:
        if modified_headers is None: modified_headers = self.get_modified_headers()
        html = self.lang == 'md'
        return max(
            [len(self.highlight_value(self.format_value(v), html)) for line in self.table.lines for v in line] +
            [len(self.highlight_text(h, html)) for h in [self.table.corner_header] + [h[0] for h in modified_headers] + self.table.lines_headers])
    
    def format_value(self, value: float) -> str:
        return "%01.4f"%value
    def highlight_value(self, value: str, html_override: bool = False) -> str:
        if html_override:       return f"<u>{value}</u>"
        if self.lang == 'tex': return "\\underline{" + value + "}"
        if self.lang == 'md': return f"<ins>{value}</ins>"
        return value
    def highlight_text(self, text: str, html_override: bool = False) -> str:
        if html_override:       return f"<strong>{text}</strong>"
        if self.lang == 'tex':  return "\\textbf{" + text + "}"
        if self.lang == 'md':   return f"**{text}**"
        return text
    
    def generate_text(self) -> str:
        return ""

    def save(self, path: str):
        with open(path, 'w') as f: f.write(self.generate_text())
class TableMarkdownSaver(TableSaver):
    def __init__(self, table: Table, **kwargs):
        super().__init__(table, 'md', **kwargs)
    
    def generate_text(self) -> str:
        md_headers: str = ""
        if (self.table.get_md_title() is not None): md_headers += self.highlight_text(self.table.get_md_title()) + "\n"
        
        modified_headers: list[tuple[str, str, int, int]] = self.get_modified_headers()
        categories: list[tuple[str, int]] = self.get_categories(modified_headers)
        cell_size: int = self.get_cell_size(modified_headers)
        
        md_headers += """
<table>
<thead>
<tr>
"""
        md_headers += f"<th> {add_spaces("", cell_size)} </th> "
        for category, size in categories:
            if size == 1:   md_headers += f"<th> {add_spaces(category, cell_size)} </th> "
            else:           md_headers += f"<th colspan=\"{size}\"> {add_spaces(category, cell_size * size + len(" </th> <th> ") * (size - 1) - len(f" colspan=\"{size}\""))} </th> "
        
        md_headers += "\n</tr>\n<tr>\n"
        
        for h in [self.table.corner_header] + [h[0] for h in modified_headers]:
            md_headers += f"<th> {add_spaces(h, cell_size)} </th> "
                                 
        md_headers += "\n </tr>\n</thead>\n"


        md_body: str = "</tbody>"
        for header, line in zip(self.table.lines_headers, self.table.lines, strict=True):
            md_body += "<tr>\n"
            md_body += f"<th> {add_spaces(self.highlight_text(header.replace("_", " "), html_override=True), cell_size)} </th> "
            
            for _, _, _, index in modified_headers:
                value = line[index]
                temp = self.format_value(value)
                if self.table.do_highlight(value, line): temp = self.highlight_value(temp)
                md_body += f"<td> {add_spaces(temp, cell_size)} </td> "
            md_body += "\n</tr>\n"
        md_body += "</tbody>\n"
        
        md_footer: str = "</table>"

        return md_headers + md_body + md_footer
class TableTexSaver(TableSaver):
    def __init__(self, table: Table, **kwargs):
        super().__init__(table, 'tex', **kwargs)
    
    def generate_text(self) -> str:
        tex_headers: str = "\\begin{table}[ht]\n"
        if (self.table.get_tex_title() is not None): tex_headers += "\\caption{" + self.table.get_tex_title() + "}\n"
        if (self.table.get_tex_label() is not None): tex_headers += "\\label{" + self.table.get_tex_label() + "}"
        
        modified_headers: list[tuple[str, str, int, int]] = self.get_modified_headers()
        categories: list[tuple[str, int]] = self.get_categories(modified_headers)
        cell_size: int = self.get_cell_size(modified_headers)
        
        tex_headers += """
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
"""
        tex_headers += "\\begin{tabular}{l |" + " | ".join([" ".join("c" * c[1]) for c in categories]) + "}\n"
        tex_headers += "\\hline\n"
        tex_headers += add_spaces("", cell_size)
        for category, size in categories:
            if size == 1:   tex_headers += f" & {add_spaces(category, cell_size)}"
            else:           tex_headers += " & " + add_spaces("\\multicolumn{" + str(size) + "}{" + ("c|" if category != categories[-1][0] else "c") + "}{" + category + "}", cell_size * size + len(" & ") * (size - 1))
        tex_headers += "\\\\ \n"
        tex_headers += " & ".join([add_spaces(h, cell_size) for h in [self.table.corner_header] + [h[0] for h in modified_headers]])
        tex_headers += "\\\\ \n"
        tex_headers += "\\hline\n"


        tex_body: str = ""
        current_n: str = "-1"
        for header, line in zip(self.table.lines_headers, self.table.lines, strict=True):
            new_n: str = header.split("_")[0]
            if (current_n != new_n):
                tex_body += "\\hline\n"
                current_n = new_n
            
            tex_body += add_spaces(self.highlight_text(header.replace("_", " ")), cell_size)
            
            for _, _, _, index in modified_headers:
                value = line[index]
                temp = self.format_value(value)
                if self.table.do_highlight(value, line): temp = self.highlight_value(temp)
                tex_body += " & " + add_spaces(temp, cell_size)
            tex_body += "\\\\\n"
        tex_body += "\\hline\n"
        
        
        tex_footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""

        return tex_headers + tex_body + tex_footer

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
    
    def __init__(self, data_loaders: list[DataLoader], algo_list: list[Algo], budget: int):
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

def ensure_load(data_loaders: list[DataLoader], algo_list: list[Algo]):
    def load(data_loader: DataLoader, keys, algo: Algo):
        data_loader.get_with_parameters(algo.get_algo(), **keys).get_anytime_scores(100)
        data_loader.get_with_parameters(algo.get_algo(), **keys).get_avg_run_score()

    threads = []
    
    for data_loader in data_loaders:
        for instance, keys in data_loader.get_parameters_iterator():
            # load(data_loader, keys, algo_list)
            for algo in algo_list:
                t = threading.Thread(target=load, args=(data_loader,keys,algo))
                threads.append(t)

    # Start each thread
    for t in threads:
        t.start()

    for i in tqdm.tqdm(range(len(threads))):
        threads[i].join()

def generate_all_tables(data_loaders: dict[str, DataLoader], output_path: str, **kwargs):
    def hc_selector(algo: str) -> bool:
        return (algo.startswith("hc_")
                and "cycle" not in algo
                and "first" not in algo
        )
    def gj_selector(algo: str) -> bool:
        return (algo.startswith("greedy_")
                and "median" not in algo
                and "middle" not in algo
                and "middle2" not in algo
                and "fixed" not in algo
                and "random" not in algo
                and "adaptative" not in algo
                and "guided" not in algo
                and "tabu" not in algo
                and "lambda" not in algo
                and "Asc+" not in algo and "Asc-" not in algo
                and "Rand+" not in algo and "Rand-" not in algo
        )
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
    print([algo.get_full_label() for algo in algo_list])
    
    table_data_loaders: list[DataLoader] = [data_loaders["NK"], data_loaders["Sat"], data_loaders["Qubo"]]
    
    ensure_load(table_data_loaders, algo_list)
    
    table = AvgScoreTable(table_data_loaders, algo_list)
    TableMarkdownSaver(table, **kwargs).save(output_path + "/avg.md")
    TableTexSaver(table, **kwargs).save(output_path + "/avg.tex")
    
    table = AvgBudgetTable(table_data_loaders, algo_list)
    TableMarkdownSaver(table, **kwargs).save(output_path + "/budgets.md")
    TableTexSaver(table, **kwargs).save(output_path + "/budgets.tex")
    
    for budget in [1_000, 10_000, 100_000, 1_000_000]:
        table = MaxScoreTable(table_data_loaders, algo_list, budget)
        TableMarkdownSaver(table, **kwargs).save(output_path + f"/best_{budget:_}.md")
        TableTexSaver(table, **kwargs).save(output_path + f"/best_{budget:_}.tex")
