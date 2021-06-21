<p align="center"><a href="https://github.com/piotrek-szczygiel/predator-prey-simulator"><img src="./icon.svg" height="150" alt="Icon"></a></p>
<h1 align="center">Predator-Prey simulator</h1>
<p align="center">
    <a href="https://github.com/piotrek-szczygiel/predator-prey-simulator/actions/workflows/cmake.yml">
        <img src="https://github.com/piotrek-szczygiel/sim/actions/workflows/cmake.yml/badge.svg">
    </a>
    <a href="https://github.com/piotrek-szczygiel/predator-prey-simulator/releases">
        <img src="https://img.shields.io/github/v/release/piotrek-szczygiel/predator-prey-simulator?include_prereleases&label=version">
    </a>
    <a href="https://github.com/piotrek-szczygiel/predator-prey-simulator/releases">
        <img src="https://img.shields.io/github/downloads/piotrek-szczygiel/predator-prey-simulator/total">
    </a>
    <a href="https://github.com/piotrek-szczygiel/predator-prey-simulator/releases/latest">
        <img src="https://img.shields.io/github/release-date-pre/piotrek-szczygiel/predator-prey-simulator?label=last%20release">
    </a>
    <a href="https://github.com/piotrek-szczygiel/predator-prey-simulator/commits/master">
        <img src="https://img.shields.io/github/last-commit/piotrek-szczygiel/predator-prey-simulator">
    </a>
</p>
<hr>

![screenshot](screen.png)

<p align="center">Population plot created with <code>run_and_plot.py</code></p>

![plot](plot.png)

# Building

Required programs:

- C++ compiler
- cmake

## Downloading submodules

```bash
git submodule update --init --recursive
```

## Installing Ubuntu dependencies

```bash
sudo apt install \
    build-essential git cmake libasound2-dev mesa-common-dev libx11-dev \
    libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev libluajit-5.1-dev
```

## Running CMake
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

# Running Predator-Prey simulator
```bash
./sim # Launch graphical interface

./sim 1000 # Run simulation 1000 times and output CSV statistics to stdout

# Helper script to create 10 plots of 1000 tick simulations and launch them as 4 parallel processes
python run_and_plot.py sim.exe 1000 default.lua 10 4
```

# Attributions

Textures by [Franuka](https://franuka.itch.io/).

Icon made by [Freepik](https://www.freepik.com/) from [Flaticon](https://www.flaticon.com/).
