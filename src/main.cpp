#include <memory>
#include "platform.h"

#include <gif.h>
#include <stb_image.h>

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

int run_zarzecze(const char* script, const char* predator_file, const char* prey_file) {
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

    int x, y, n;
    uint8_t* predator_img = stbi_load(predator_file, &x, &y, &n, 4);
    if (!predator_img) {
        fprintf(stderr, "unable to load image: %s\n", predator_file);
        return 5;
    }

    if (x != config.tile_size || y != config.tile_size) {
        fprintf(stderr, "invalid size of %s: %dx%d\n", predator_img, x, y);
        return 6;
    }

    uint8_t* prey_img = stbi_load(prey_file, &x, &y, &n, 4);
    if (!prey_img) {
        fprintf(stderr, "unable to load image: %s\n", predator_file);
        return 5;
    }

    if (x != config.tile_size || y != config.tile_size) {
        fprintf(stderr, "invalid size of %s: %dx%d\n", prey_img, x, y);
        return 6;
    }

    int tile = (int)config.tile_size;
    int image_w = tile * config.sim_width;
    int image_h = tile * config.sim_height;
    int image_sz = image_w * image_h * 4;

    uint8_t* grass_img = (uint8_t*)malloc(image_sz);

    for (int i = 0; i < image_sz; i += 4) {
        grass_img[i] = 0;
        grass_img[i + 1] = 158;
        grass_img[i + 2] = 47;
        grass_img[i + 3] = 255;
    }

    int delay = 5;

    uint8_t* frame = (uint8_t*)malloc(image_sz);

    GifWriter gif;
    GifBegin(&gif, "output.gif", image_w, image_h, delay);

    int max = 10000;
    for (int i = 0; i < max; ++i) {
        sim.update(scripting);

        for (int j = 0; j < image_sz; j += 4) {
            frame[j] = 99;
            frame[j + 1] = 171;
            frame[j + 2] = 63;
            frame[j + 3] = 255;
        }

        for (const auto& chunk : sim.chunks()) {
            for (const auto& agent : chunk) {
                if (agent.is_dead()) continue;

                int tile_i = 0;
                int frame_i = image_w * tile * 4 * agent.pos.y + agent.pos.x * tile * 4;
                int skip = image_w * 4;

                uint8_t* draw = nullptr;
                switch (agent.type) {
                    case AgentType::Wolf: draw = predator_img; break;
                    case AgentType::Chicken: draw = prey_img; break;
                    case AgentType::Grass: draw = grass_img; break;
                }

                for (int y = 0; y < tile; ++y) {
                    for (int x = 0; x < 4 * tile; x += 4) {
                        uint8_t alpha = draw[tile_i + x + 3];
                        if (alpha == 0) continue;
                        frame[frame_i + x] = draw[tile_i + x];
                        frame[frame_i + x + 1] = draw[tile_i + x + 1];
                        frame[frame_i + x + 2] = draw[tile_i + x + 2];
                        frame[frame_i + x + 3] = alpha;
                    }
                    frame_i += skip;
                    tile_i += 4 * tile;
                }
            }
        }

        GifWriteFrame(&gif, frame, image_w, image_h, delay);

        printf("%d / %d\n", i, max);
        if (sim.count(AgentType::Wolf) < 2) break;
    }

    GifEnd(&gif);

    free(frame);
    free(grass_img);
    stbi_image_free(predator_img);
    stbi_image_free(prey_img);

    return 0;
}

int main(int argc, char** argv) {
    bool graphics = argc == 1;
    bool zarzecze = false;

    Tick sim_ticks = 0;
    const char* script = "assets/scripts/default.lua";

    if (argc >= 2) {
        if (strcmp(argv[1], "zarzecze") == 0) {
            zarzecze = true;
            if (argc != 4) {
                fprintf(stderr, "usage: %s zarzecze <predator_image> <prey_image>\n", argv[0]);
                return 1;
            }
        } else {
            char* p_end;
            sim_ticks = std::strtoll(argv[1], &p_end, 10);
            if (p_end == argv[1]) {
                fprintf(stderr, "usage: %s <simulation_ticks> [script]\n", argv[0]);
                return 1;
            }

            if (argc == 3) script = argv[2];
        }
    }

    if (!cd_assets()) {
        fprintf(stderr, "unable to find assets directory\n");
        if (graphics) return 2;
    }

    if (zarzecze) return run_zarzecze(script, argv[2], argv[3]);
    if (graphics) return run_graphics();
    return run_csv(sim_ticks, script);
}
