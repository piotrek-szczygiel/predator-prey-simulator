#!/usr/bin/env python3
import os
import pathlib
import shutil

import pandas as pd
import matplotlib.pyplot as plt


def collect_files(roots, extensions):
    result = []
    for root in roots:
        for (directory, _, files) in os.walk(root):
            if ".pio" in directory:
                continue

            for f in files:
                path = os.path.join(directory, f)
                if pathlib.Path(path).suffix in extensions:
                    result.append(path)

    return result


def plot(file):
    df = pd.read_csv(file, sep=";")
    x = range(df.shape[0])

    plt.figure()
    plt.plot(x, df["predator"], label="predator")
    plt.plot(x, df["prey"], label="prey")

    plt.xlabel("simulation step")
    plt.ylabel("population")

    plt.legend()

    basename = os.path.splitext(os.path.basename(file))[0]
    plt.savefig(f"png/{basename}.png", dpi=300)


if __name__ == "__main__":
    shutil.rmtree("png", ignore_errors=True)
    os.mkdir("png")
    for f in collect_files(["csv"], [".csv"]):
        plot(f)
