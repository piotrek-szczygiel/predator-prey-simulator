#include <iostream>
#include "platform.h"

int main(int argc, char** argv) {
    bool graphics = argc == 1;
    size_t sim_ticks = 0;

    if (argc == 2) {
        char* p_end;
        sim_ticks = std::strtoull(argv[1], &p_end, 10);
        if (p_end == argv[1]) {
            std::cerr << "usage: " << argv[0] << " <ticks>\n";
            return 2;
        }
    }

    if (!cd_assets()) {
        std::cerr << "unable to find assets directory\n";
        if (graphics) return 1;
    }

    Simulation sim(WIDTH, HEIGHT);

    if (graphics) {
        Platform p(900, 900, "Predator Prey Simulation", 240);
        p.start(WIDTH, HEIGHT);

        while (Platform::running()) {
            p.start_drawing(sim);
            sim.update();
            p.end_drawing();
        }

        p.stop();
    } else {
        printf("chicken,wolf,cabbage\n");
        while (sim.ticks() < sim_ticks) {
            sim.update();
            int chicken = sim.count(AgentType::Chicken);
            int wolf = sim.count(AgentType::Wolf);
            int cabbage = sim.count(AgentType::Cabbage);
            printf("%d,%d,%d\n", chicken, wolf, cabbage);
        }
    }

    return 0;
}