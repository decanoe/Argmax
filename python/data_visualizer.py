import os
from tables import generate_all_tables
from data_loader import NKDataLoader, SatDataLoader, QuboDataLoader
import matplotlib.pyplot as plt
from window import Window

if __name__ != "__main__":
    exit()
dir_path = os.path.dirname(os.path.realpath(__file__))

plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

directory: str = dir_path+"/../rundata/local_search/b1_000_000"
data_loaders = {
    "NK": NKDataLoader(directory),
    "Sat": SatDataLoader(directory),
    "Qubo": QuboDataLoader(directory),
}

generate_all_tables(data_loaders, dir_path + "/output")
exit(0)

fig, _ = plt.subplots()
Window(data_loaders, fig)