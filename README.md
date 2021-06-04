# sim

![cmake](https://github.com/piotrek-szczygiel/sim/actions/workflows/cmake.yml/badge.svg)


Predator-Prey simulation

## Requirements

- C++ compiler
- cmake

### Ubuntu

`sudo apt install build-essential git cmake libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev`

## Building
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Running
```bash
./sim # Launch graphical interface

./sim 1000 # Run simulation 1000 times and output CSV statistics to stdout

# Helper script to create 10 plots of 1000 tick simulations and launch them as 4 parallel processes
python run_and_plot.py build/sim 1000 10 4
```
