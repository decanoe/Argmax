# region output_table
def add_spaces(value: float, str_size: int):
    return str(value) + " " * (str_size - len(str(value)))

COUNT_PER_INSTANCE: int = 2 # nb of runs to take per instance (should not exceed the number of runs done on each instances)

def generate_avg_table_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(data_loader.get_reference_file(algo).label, CELL_SIZE) for algo in ["instance (N_K)"] + data_loader.Algo_keys])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(data_loader.Algo_keys) + 1))
    
    for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            values = []
            for algo in data_loader.Algo_keys:
                algo_data = k_data[algo]
                values.append(0)
                count = 0
                for i, i_data in algo_data.items():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                    nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                    if (nb_ends < COUNT_PER_INSTANCE):
                        print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                    
                    count += nb_ends
                    values[-1] += ends.fitness_after_jump[:nb_ends].sum()
                values[-1] /= count
                values[-1] = round(values[-1], 4)
            
            for value in values:
                if value == max(values):
                    avg_content += " | " + add_spaces(f"<ins>{value}</ins>", CELL_SIZE)
                else:
                    avg_content += " | " + add_spaces(value, CELL_SIZE)
            
    with open(dir_path + "/output/avg.txt", "w") as f: f.write(avg_content + " |")
def generate_avg_table_latex():
    CELL_SIZE = 7
    
    header: str = """\\begin{table}[ht]
\\caption{Mean fitness score per execution. Results are averaged over """ + str(COUNT_PER_INSTANCE * 10) + """ instances per $(N,K)$ pair. The best results for each instance are underlined.}
\\label{tab:mean_scores}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
    footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""
    
    body: str = ""
    
    for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            body += "\\textbf{" + str(n) + ", " + str(k) + "} "
            line_values = []
            
            for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                for algo_param in ["best", "first", "least"]:
                    algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                    algo_data = k_data[algo]
                    value: float = 0
                    count: int = 0
                    for i, i_data in algo_data.items():
                        ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                        nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                        if (nb_ends < COUNT_PER_INSTANCE):
                            print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                        
                        count += nb_ends
                        value += ends.fitness_after_jump[:nb_ends].sum()
                    line_values.append(round(value / count, 4))
            
            for value in line_values:
                if value == max(line_values):
                    body += "& " + add_spaces("\\underline{" + "%01.4f"%value + "}", CELL_SIZE) + " "
                else:
                    body += "& " + add_spaces("%01.4f"%value, CELL_SIZE) + " "
            body += " \\\\\n"
        body += "\\hline\n"
    
    with open(dir_path + "/output/avg.tex", "w") as f: f.write(header + body + footer)

def generate_budget_tables_md():
    CELL_SIZE = 16
    
    header: str = "| " + " | ".join([add_spaces(data_loader.get_reference_file(algo).label, CELL_SIZE) for algo in ["instance (N_K)"] + data_loader.Algo_keys])
    header += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(data_loader.Algo_keys) + 1))
    budgets: list[int] = [100_000, 10_000, 1_000]
    best_budget_content: list[str] = [header for _ in budgets]
    
    for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            for budget_index in range(len(budgets)):
                best_budget_content[budget_index] += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
                values = []
                for algo in data_loader.Algo_keys:
                    algo_data = k_data[algo]
                    values.append(0)
                    for i_data in algo_data.values():
                        values[-1] += i_data.data[i_data.data.budget <= budgets[budget_index]].fitness_after_jump.max()
                    values[-1] /= len(algo_data.values())
                    values[-1] = round(values[-1], 4)
                
                for value in values:
                    if value == max(values):
                        best_budget_content[budget_index] += " | " + add_spaces(f"<ins>{"%01.4f"%value}</ins>", CELL_SIZE)
                    else:
                        best_budget_content[budget_index] += " | " + add_spaces("%01.4f"%value, CELL_SIZE)
    
    for budget_index in range(len(budgets)):
        with open(dir_path + f"/output/best_{budgets[budget_index]//1000}_000.txt", "w") as f: f.write(best_budget_content[budget_index] + " |")
def generate_budget_tables_latex():
    CELL_SIZE = 9
    
    budgets: list[int] = [1_000, 10_000, 100_000]
    budgets_text = [
        "Best fitness achieved under a limited budget of 1,000 evaluations. Underlined values indicate the best performance for each instance.",
        "Best score obtained for a budget of 10,000 evaluations.",
        "Best fitness achieved under a large budget of 100,000 evaluations. Underlined values indicate the best performance for each instance."
    ]
    budgets_label = ["tab:budget1k", "tab:budget10k", "tab:budget100k"]
    
    for budget, budget_text, budget_label in zip(budgets, budgets_text, budgets_label):
        header: str = """\\begin{table}[h]
\\caption{""" + budget_text + """}
\\label{""" + budget_label + """}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
# \\textbf{50, 0} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} & \\underline{0.6573} \\\\
# \\textbf{50, 1} & 0.6988 & 0.6980 & \\underline{0.6995} & 0.6994 & 0.6989 & 0.6954 & 0.6972 & 0.6979 & 0.6867 \\\\
# \\textbf{50, 2} & 0.7422 & \\underline{0.7442} & 0.7424 & 0.7402 & 0.7441 & 0.7349 & 0.7358 & 0.7390 & 0.7210 \\\\
# \\textbf{50, 4} & 0.7558 & \\underline{0.7621} & 0.7596 & 0.7538 & 0.7618 & 0.7611 & 0.7456 & 0.7536 & 0.7212 \\\\
# \\textbf{50, 8} & 0.7532 & 0.7464 & 0.7347 & 0.7523 & 0.7500 & 0.7476 & 0.7449 & \\underline{0.7560} & 0.7026 \\\\
# \\hline
# \\textbf{100, 0} & \\underline{0.6643} & 0.6558 & 0.6268 & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} & \\underline{0.6643} \\\\
# \\textbf{100, 1} & 0.6971 & 0.6866 & 0.6515 & 0.6971 & \\underline{0.6972} & 0.6877 & 0.6908 & 0.6929 & 0.6763 \\\\
# \\textbf{100, 2} & 0.7379 & 0.7272 & 0.6947 & \\underline{0.7398} & 0.7394 & 0.7308 & 0.7297 & 0.7371 & 0.7109 \\\\
# \\textbf{100, 4} & 0.7458 & 0.7642 & 0.7014 & 0.7492 & \\underline{0.7656} & 0.7574 & 0.7368 & 0.7528 & 0.5899 \\\\
# \\textbf{100, 8} & 0.7348 & 0.7346 & 0.6677 & 0.7328 & 0.7405 & \\underline{0.7441} & 0.7207 & 0.7413 & 0.5444 \\\\
# \\hline
        footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""

        body: str = ""
    
        for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
            for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
                body += "\\textbf{" + str(n) + ", " + str(k) + "} "
                line_values = []
                
                for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                    for algo_param in ["best", "first", "least"]:
                        algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                        algo_data = k_data[algo]
                        
                        line_values.append(0)
                        for i_data in algo_data.values():
                            line_values[-1] += i_data.data[i_data.data.budget <= budget].fitness_after_jump.max()
                        line_values[-1] /= len(algo_data.values())
                        line_values[-1] = round(line_values[-1], 4)
                
                for value in line_values:
                    if value == max(line_values):
                        body += "& " + add_spaces("\\underline{" + "%01.4f"%value + "}", CELL_SIZE) + " "
                    else:
                        body += "& " + add_spaces("%01.4f"%value, CELL_SIZE) + " "
                body += " \\\\\n"
            body += "\\hline\n"
        
        with open(dir_path + f"/output/best_{budget//1000}_000.tex", "w") as f: f.write(header + body + footer)

def avg_budget_per_algo_md():
    CELL_SIZE = 16
    
    avg_content: str = "| " + " | ".join([add_spaces(data_loader.get_reference_file(algo).label, CELL_SIZE) for algo in ["instance (N_K)"] + data_loader.Algo_keys])
    avg_content += " |\n| " + " | ".join(["-" * CELL_SIZE] * (len(data_loader.Algo_keys) + 1))
    
    for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            avg_content += " |\n| " + add_spaces(f"**{n}_{k}**", CELL_SIZE)
            line_values = []
            
            for algo in data_loader.Algo_keys:
                algo_data = k_data[algo]
                value: float = 0
                count: int = 0
                for i, i_data in algo_data.items():
                    ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                    nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                    if (nb_ends < COUNT_PER_INSTANCE):
                        print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                    
                    count += nb_ends
                    value += float(ends.in_run_budget[:nb_ends].sum())
                line_values.append(round(value / count, 1))
            
            for value in line_values:
                if value == min(line_values):
                    avg_content += " | " + add_spaces("<ins>" + "%.1f"%value + "</ins>", CELL_SIZE)
                else:
                    avg_content += " | " + add_spaces("%.1f"%value, CELL_SIZE)
    
    with open(dir_path + "/output/budgets.txt", "w") as f: f.write(avg_content + " |")
def avg_budget_per_algo_latex():
    CELL_SIZE = 7
    
    header: str = """\\begin{table}[ht]
\\caption{Mean evaluation budget per single trajectory. Data averaged over """ + str(COUNT_PER_INSTANCE * 10) + """ independent runs per $(N,K)$ pair across 10 problem instances.}
\\label{tab:mean_budget}
\\centering
\\setlength{\\tabcolsep}{4pt} % Ajustement de l'espacement des colonnes
\\begin{scriptsize}
\\begin{tabular}{l | c c c | c c c | c c c}
\\hline
Instances & \\multicolumn{3}{c|}{\\Full} & \\multicolumn{3}{c|}{\\Improve} & \\multicolumn{3}{c}{\\HC} \\\\
$(N,K)$ & \\Best & \\First & \\Least & \\Best & \\First & \\Least & \\Best & \\First & \\Least \\\\
\\hline
"""
    footer: str = """\\end{tabular}
\\end{scriptsize}
\\end{table}"""
    
    body: str = ""
    
    for n, n_data in sorted(data_loader.file_infos.items(), key=lambda p : p[0]):
        for k, k_data in sorted(n_data.items(), key=lambda p : p[0]):
            body += "\\textbf{" + str(n) + ", " + str(k) + "} "
            line_values = []
            
            for algo_type in ["greedy_all_", "greedy_improve_", "hc_"]:
                for algo_param in ["best", "first", "least"]:
                    algo: str = (algo_type + algo_param).replace("hc_first", "hc_random")
                    algo_data = k_data[algo]
                    value: float = 0
                    count: int = 0
                    for i, i_data in algo_data.items():
                        ends = i_data.data[(i_data.data.size_of_the_jump == 0) * (i_data.data.in_run_budget != 1) * (i_data.data.budget != i_data.data.budget.max())]
                        nb_ends = min(COUNT_PER_INSTANCE, len(ends))
                        if (nb_ends < COUNT_PER_INSTANCE):
                            print(f"Warning : not enough ends to do budget average on {n},{k} {algo}_{i} (only got {nb_ends})")
                        
                        count += nb_ends
                        value += float(ends.in_run_budget[:nb_ends].sum())
                    line_values.append(round(value / count, 1))
            
            for value in line_values:
                if value == min(line_values):
                    body += "& " + add_spaces("\\underline{" + "%.1f"%value + "}", CELL_SIZE) + " "
                else:
                    body += "& " + add_spaces("%.1f"%value, CELL_SIZE) + " "
            body += " \\\\\n"
        body += "\\hline\n"
    
    with open(dir_path + "/output/budgets.tex", "w") as f: f.write(header + body + footer)

# avg_budget_per_algo_md()
# avg_budget_per_algo_latex()

# generate_avg_table_md()
# generate_avg_table_latex()

# generate_budget_tables_md()
# generate_budget_tables_latex()
# endregion
