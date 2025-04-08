import sys
import ast
import pandas as pd

from file_selector import file_selector

if __name__ != "__main__":
    exit()
    
data_file: str = ""
if (len(sys.argv) < 2):
    data_file = file_selector()
else:
    data_file = sys.argv[1]

problem: str = data_file.split("_")[0]

data = pd.read_csv(data_file, sep = "\t", index_col = "generation")
data.population = data.population.map(lambda s: ast.literal_eval(s.replace(",]", "]")))
print(data)