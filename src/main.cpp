#include "platform.h"

int run_graphics() {
    Config config{};
    if (!config.load("config.ini")) {
        fprintf(stderr, "error while reading config.ini\n");
        return 3;
    }

    Platform p(config);
    p.start();

    bool restart = false;
    do {
        if (restart) {
            restart = false;
            if (!config.load("config.ini")) {
                fprintf(stderr, "error while reloading config.ini\n");
                return 4;
            }

            p.reload();
        }

        Simulation sim(config);

        float last_update = p.time();
        while (!p.should_close() && !restart) {
            restart = p.should_restart();
            p.interact();
            p.start_drawing(sim);

            if (config.window_tick_time_ms != -1) {
                if (p.time() - last_update >= (float)config.window_tick_time_ms / 1000.0f) {
                    sim.update();
                    last_update = p.time();
                }
            } else if (p.should_tick()) {
                sim.update();
            }

#ifndef NDEBUG
            sim.draw_debug();
#endif
            p.end_drawing();
        }
    } while (restart);

    p.stop();
    return 0;
}

int run_csv(size_t sim_ticks) {
    Config config{};
    if (!config.load("config.ini")) {
        fprintf(stderr, "error while reading config.ini\n");
        return 3;
    }

    Simulation sim(config);

    printf("chicken,wolf,cabbage\n");
    while (sim.ticks() < sim_ticks) {
        sim.update();
        int chicken = sim.count(AgentType::Chicken);
        int wolf = sim.count(AgentType::Wolf);
        int cabbage = sim.count(AgentType::Cabbage);
        printf("%d,%d,%d\n", chicken, wolf, cabbage);
    }

    return 0;
}

int main(int argc, char** argv) {
    bool graphics = argc == 1;
    size_t sim_ticks = 0;

    if (argc == 2) {
        char* p_end;
        sim_ticks = std::strtoull(argv[1], &p_end, 10);
        if (p_end == argv[1]) {
            fprintf(stderr, "usage: %s simulation_ticks\n", argv[0]);
            return 1;
        }
    }

    if (!cd_assets()) {
        fprintf(stderr, "unable to find assets directory\n");
        if (graphics) return 2;
    }

    if (graphics) {
        return run_graphics();
    } else {
        return run_csv(sim_ticks);
    }
}