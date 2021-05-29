#include <raylib.h>
#include <iostream>
#include "platform.h"
#include "simulation.h"
#include "util.h"

int main(int argc, char** argv) {
    if (!cd_assets()) {
        std::cerr << "unable to find assets directory\n";
        return 1;
    }

    Platform p(900, 900, "Predator Prey Simulation", FPS);
    p.start(WIDTH, HEIGHT);

    Simulation sim(WIDTH, HEIGHT);

    while (p.running()) {
        sim.update();
        printf("count: %d\n", sim.count(AgentType::Chicken));
        p.draw(sim);
    }

    p.stop();
    return 0;
}