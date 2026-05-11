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

data_loaders = {
    "NK": NKDataLoader(dir_path+"/../rundata/local_search/b100_000"),
    # "Sat": SatDataLoader(dir_path+"/../rundata/local_search"),
    # "Qubo": QuboDataLoader(dir_path+"/../rundata/local_search"),
}

# generate_all_tables(data_loaders, dir_path + "/output")
# exit(0)

fig, _ = plt.subplots()
Window(data_loaders, fig)