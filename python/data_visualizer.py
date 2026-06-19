import os
from typing import Any
from tables import generate_all_tables
from csv_generator import generate_all_non_iterated_csv, generate_all_iterated_csv
from data_loader import NKDataLoader, SatDataLoader, QuboDataLoader
import matplotlib.pyplot as plt
from window import Window
import sys

if __name__ != "__main__":
    exit()

dir_path = os.path.dirname(os.path.realpath(__file__))

plt.rcParams.update({
    "savefig.directory": dir_path.removesuffix("python") + "docs/graphs",
    "savefig.format": "pdf",
})

kwargs: dict[str, Any] = {
    # "display_ordering": True,
    # "display_aspiration": True,
    # "display_push_order": True,
}

directory: str = dir_path+"/../rundata/local_search/b1_000_000"
data_loaders = {
    "NK": NKDataLoader(directory, **kwargs),
    "Sat": SatDataLoader(directory, **kwargs),
    "Qubo": QuboDataLoader(directory, **kwargs),
}

if ("tables" in sys.argv):
    generate_all_tables(data_loaders, dir_path + "/output")
    exit(0)
if ("csv" in sys.argv):
    generate_all_non_iterated_csv(data_loaders, dir_path + "/output/csv")
    generate_all_iterated_csv(data_loaders, dir_path + "/output/csv")
    exit(0)

fig, _ = plt.subplots()
Window(data_loaders, fig, **kwargs)