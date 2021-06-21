#include <memory>
#include "platform.h"

int run_graphics() {
    Config config("config.ini");
    if (!config.load()) {
        fprintf(stderr, "error while reading config.ini\n");
        return 3;
    }

    Platform p(config);
    p.start();

    bool restart = false;
    do {
        if (restart) {
            restart = false;
            p.reload();
            p.gui().set_status("Simulation restarted!");
        }

        auto scripting = std::make_unique<Scripting>();
        if (!scripting->init()) {
            fprintf(stderr, "error while initializing scripting engine\n");
            p.gui().set_status("Unable to initialize scripting engine!");
            return 4;
        }

        Simulation sim(config);

        auto current_script = p.gui().selected_script();
        if (scripting->load(current_script, sim)) {
            fprintf(stderr, "script %s successfully loaded\n", current_script.c_str());
            p.gui().set_status("Script " + current_script + " successfully loaded!");
        } else {
            fprintf(stderr, "error while loading script %s\n", current_script.c_str());
            p.gui().set_status("Unable to load script " + current_script + "!");
        }

        sim.update(*scripting);
        auto last_update = p.time_now();

        while (!p.should_close() && !restart) {
            p.interact(sim);
            restart = p.should_restart();
            p.start_drawing(sim);

            bool update = false;
            if (!config.control_pause) {
                if (p.time_diff_ms(p.time_now(), last_update) >= (double)config.control_tick_time) {
                    update = true;
                    last_update = p.time_now();
                }
            } else if (p.should_tick()) {
                update = true;
            }

            if (update) {
                sim.update(*scripting);
                p.gui().plot_add(sim.count(AgentType::Chicken), sim.count(AgentType::Wolf));
            }

            if (config.control_debug) p.draw_debug(sim);
            p.update_gui_end_drawing(sim);

            if (p.should_reload_script()) {
                scripting = std::make_unique<Scripting>();

                auto selected_script = p.gui().selected_script();
                if (scripting->load(selected_script, sim)) {
                    fprintf(stderr, "script %s successfully loaded\n", selected_script.c_str());
                    p.gui().set_status("Script " + selected_script + " successfully loaded!");
                } else {
                    fprintf(stderr, "error while loading script %s\n", selected_script.c_str());
                    p.gui().set_status("Unable to load script " + selected_script + "!");
                }
            }
        }
    } while (restart);

    p.stop();
    return 0;
}

int run_csv(Tick sim_ticks, const char* script) {
    Config config("config.ini");
    if (!config.load()) {
        fprintf(stderr, "error while reading config.ini\n");
        return 3;
    }

    Scripting scripting;
    if (!scripting.init()) {
        fprintf(stderr, "error while initializing scripting engine\n");
        return 4;
    }

    Simulation sim(config);

    if (!scripting.load(script, sim)) {
        fprintf(stderr, "error while loading script!\n");
        return 4;
    }

    printf("%u\n", sim.seed());
    printf("chicken,wolf,avg_offsprings,avg_sensor_range\n");

    while (sim.ticks() < sim_ticks) {
        sim.update(scripting);

        int chicken = sim.count(AgentType::Chicken);
        int wolf = sim.count(AgentType::Wolf);
        auto [avg_offsprings, avg_sensor_range] = sim.avg_genes();
        printf("%d,%d,%f,%f\n", chicken, wolf, avg_offsprings, avg_sensor_range);

        if (wolf < 2) break;
    }

    return 0;
}

int main(int argc, char** argv) {
    bool graphics = argc == 1;
    Tick sim_ticks = 0;
    const char* script = "assets/scripts/default.lua";

    if (argc == 2 || argc == 3) {
        char* p_end;
        sim_ticks = std::strtoll(argv[1], &p_end, 10);
        if (p_end == argv[1]) {
            fprintf(stderr, "usage: %s <simulation_ticks> [script]\n", argv[0]);
            return 1;
        }

        if (argc == 3) script = argv[2];
    }

    if (!cd_assets()) {
        fprintf(stderr, "unable to find assets directory\n");
        if (graphics) return 2;
    }

    if (graphics) {
        return run_graphics();
    } else {
        return run_csv(sim_ticks, script);
    }
}
