#!/usr/bin/env python3
import os
import pathlib
import shutil
import subprocess
import sys

import matplotlib.pyplot as plt
import pandas as pd


def collect_files(roots, extensions):
    result = []
    for root in roots:
        for (directory, _, files) in os.walk(root):
            for f in files:
                path = os.path.join(directory, f)
                if pathlib.Path(path).suffix in extensions:
                    result.append(path)

    return result


def plot(file):
    df = pd.read_csv(file)
    x = range(df.shape[0])

    plt.figure()
    plt.plot(x, df["chicken"], label="chicken")
    plt.plot(x, df["wolf"], label="wolf")
    plt.plot(x, df["grass"], label="grass")

    plt.xlabel("simulation step")
    plt.ylabel("population")

    plt.legend()

    basename = os.path.splitext(os.path.basename(file))[0]
    outfile = f"png/{basename}.png"
    print(f"plot {file} > {outfile}")
    plt.savefig(outfile, dpi=300)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            f"usage: {sys.argv[0]} <iterations> <ticks> <sim executable>",
            file=sys.stderr,
        )
        sys.exit(1)

    iterations = int(sys.argv[1])
    ticks = int(sys.argv[2])
    sim_exe = sys.argv[3]

    shutil.rmtree("csv", ignore_errors=True)
    os.mkdir("csv")

    for i in range(iterations):
        with open(f"csv/{i}.csv", "w") as outfile:
            cmd = [sim_exe, str(ticks)]
            print(f"{cmd} > {outfile.name}")
            subprocess.run(cmd, stdout=outfile)

    shutil.rmtree("png", ignore_errors=True)
    os.mkdir("png")
    for f in collect_files(["csv"], [".csv"]):
        plot(f)
