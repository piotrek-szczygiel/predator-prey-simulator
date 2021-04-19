#include "ResourceManager.h"

void ResourceManager::load_all_textures() {
    m_textures.emplace(SimulationTexture::Cabbage, load_texture("cabbage.png"));
    m_textures.emplace(SimulationTexture::Chicken, load_texture("chicken.png"));
    m_textures.emplace(SimulationTexture::Ground, load_texture("ground.png"));
    m_textures.emplace(SimulationTexture::Wolf, load_texture("wolf.png"));
}

void ResourceManager::free_all_textures() {
    for (auto &texture : m_textures) {
        UnloadTexture(texture.second);
    }
    m_textures.clear();
}
Texture2D ResourceManager::get_texture(SimulationTexture texture) {
    return m_textures[texture];
}
