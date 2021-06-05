#!/usr/bin/env python3
import os
import multiprocessing
import shutil
import subprocess
import sys
import time
import functools

import matplotlib.pyplot as plt
import pandas as pd

plt.style.use("seaborn")


def plot(file):
    with open(file, "r") as f:
        seed = f.readline().strip()
        df = pd.read_csv(f)

    x = range(df.shape[0])

    plt.figure()
    plt.title(f"Seed: {seed}", fontweight="bold")
    plt.plot(x, df["chicken"], label="chicken", c="green")
    plt.plot(x, df["wolf"], label="wolf", c="red")

    plt.xlabel("tick", fontweight="bold")
    plt.ylabel("population", fontweight="bold")

    plt.legend()

    outfile = f"png/{seed}.png"
    plt.savefig(outfile, dpi=300)
    plt.clf()
    return outfile


def run(sim_exe, ticks, file):
    with open(file, "w") as f:
        subprocess.run([sim_exe, str(ticks)], stdout=f)
    return plot(file)


def main():
    if len(sys.argv) not in [4, 5]:
        print(
            f"usage: {sys.argv[0]} <sim executable> <ticks> <iterations> [processes]",
            file=sys.stderr,
        )
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
