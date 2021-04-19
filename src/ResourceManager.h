#pragma once
#include <raylib.h>
#include <map>
#include <string>

enum class SimulationTexture { Cabbage, Chicken, Ground, Wolf };

class ResourceManager {
   public:
    void load_all_textures();
    void free_all_textures();
    Texture2D get_texture(SimulationTexture texture);

    static ResourceManager& the() {
        static ResourceManager resourceManager;
        return resourceManager;
    }

   private:
    const std::string m_asset_path = "../assets/";
    std::map<SimulationTexture, Texture2D> m_textures;

    Texture2D load_texture(const std::string& path) { return LoadTexture((m_asset_path + path).c_str()); }
};
