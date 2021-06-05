#!/usr/bin/env python3
import os
import pathlib
import multiprocessing
import shutil
import subprocess
import sys
import time
import functools

import matplotlib.pyplot as plt
import pandas as pd


def plot(file, min_ticks=0):
    with open(file, "r") as f:
        seed = f.readline().strip()
        df = pd.read_csv(f)

    ticks = df.shape[0]
    if ticks < min_ticks:
        return None

    x = range(ticks)

    if ticks > 25000:
        figsize = (6.4 * ticks / 25000, 4.8)
    else:
        figsize = (6.4, 4.8)

    plt.figure(figsize=figsize)
    plt.title(f"Seed: {seed}", fontweight="bold")
    plt.plot(x, df["chicken"], label="prey", c="green")
    plt.plot(x, df["wolf"], label="predator", c="red")

    plt.xlabel("tick", fontweight="bold")
    plt.ylabel("population", fontweight="bold")

    plt.legend()

    outfile = f"png/{ticks}_{seed}.png"
    plt.savefig(outfile, dpi=300)
    plt.clf()
    return outfile


def run(sim_exe, ticks, file):
    with open(file, "w") as f:
        subprocess.run([sim_exe, str(ticks)], stdout=f)
    return plot(file)


def main():
    if len(sys.argv) == 3 and sys.argv[1] == "choose":
        min_ticks = int(sys.argv[2])
        shutil.rmtree("png", ignore_errors=True)
        os.mkdir("png")

        csv = []
        for (directory, _, files) in os.walk("csv"):
            for file in files:
                path = pathlib.Path(os.path.join(directory, file))
                if path.suffix == ".csv":
                    csv.append(path)

        plot_at_least = functools.partial(plot, min_ticks=min_ticks)
        with multiprocessing.Pool(processes=32) as pool:
            for i, png in enumerate(pool.imap_unordered(plot_at_least, csv)):
                if png:
                    print(f"[{i+1}/{len(csv)}] {png} replotted")

        return

    if len(sys.argv) not in [4, 5]:
        print(
            f"usage: {sys.argv[0]} <sim executable> <ticks> <iterations> [processes]",
            file=sys.stderr,
        )
        print(
            "\nAfter running this script you can later run this command to replot only the best seeds:"
        )
        print(f"  {sys.argv[0]} choose <min_ticks>")
        sys.exit(1)

    sim_exe = sys.argv[1]
    ticks = int(sys.argv[2])
    iterations = int(sys.argv[3])
    processes = multiprocessing.cpu_count() if len(sys.argv) == 4 else int(sys.argv[4])

    run_simulation = functools.partial(run, sim_exe, ticks)

    shutil.rmtree("csv", ignore_errors=True)
    shutil.rmtree("png", ignore_errors=True)

    os.mkdir("csv")
    os.mkdir("png")

    files = [f"csv/{i}.csv" for i in range(iterations)]

    start = time.time()
    i = 0
    with multiprocessing.Pool(processes=processes) as pool:
        for png in pool.imap_unordered(run_simulation, files):
            i += 1
            print(f"[{i}/{iterations}] {png} finished")
    elapsed = time.time() - start

    print(f"\nelapsed time: {elapsed:.3f}s")


if __name__ == "__main__":
    main()
