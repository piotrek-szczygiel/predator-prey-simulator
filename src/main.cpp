#include <raylib.h>
#include <iostream>
#include "platform.h"
#include "simulation.h"
#include "util.h"

const int SIM_WIDTH = 50;
const int SIM_HEIGHT = 50;

int main(int argc, char** argv) {
    if (!cd_assets()) {
        std::cerr << "unable to find assets directory\n";
        return 1;
    }

    Platform p(900, 900, "Predator Prey Simulation");
    p.start(SIM_WIDTH, SIM_HEIGHT);

    Simulation sim(SIM_WIDTH, SIM_HEIGHT);

    while (p.running()) {
        sim.update(p.time());
        p.draw(sim);
    }

    p.stop();
    return 0;
}