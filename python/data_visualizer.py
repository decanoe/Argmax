import os
from tables import generate_all_tables
from data_loader import DataLoader
import matplotlib.pyplot as plt
from window import Window

if __name__ != "__main__":
    exit()
dir_path = os.path.dirname(os.path.realpath(__file__))

plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

data_loader: DataLoader = DataLoader(dir_path+"/../rundata", small_load=False)

# generate_all_tables(data_loader, dir_path + "/output")
# exit(0)

fig, _ = plt.subplots()
Window(data_loader, fig)