#pragma once

#include <string>
#include <unordered_map>

#include "Logger.hpp"
#include "Texture.hpp"

namespace msgui
{
class TextureLoader
{
public:
    // Statics
    static TexturePtr loadTexture(const std::string& resPath,
        const Texture::Params& params = Texture::Params{});

private:
    TextureLoader() = default;
    TextureLoader(const TextureLoader&);
    TextureLoader& operator=(const TextureLoader&);
    TextureLoader(TextureLoader&&);
    TextureLoader& operator=(TextureLoader&&);

    // Normal
    Texture loadTextureInternal(const std::string& resPath,
        const Texture::Params& params);

    // Statics
    static TextureLoader& get();

private:
    Logger log_;
    std::unordered_map<std::string, TexturePtr> texPathToObject_;
};
} // namespace msgui