#pragma once

#include <string>
#include <unordered_map>

#include "Logger.hpp"
#include "Texture.hpp"

namespace msgui
{
/* Class that loads a texture and stores it uniquely accross windows */
class TextureLoader
{
public:
    /**
        Load a texture from path.

        @param resPath Path to resource to load from
        @return Textuure pointer
    */
    static TexturePtr loadTexture(const std::string& resPath,
        const Texture::Params& params = Texture::Params{});

private:
    /* Cannot be copied or moved */
    TextureLoader() = default;
    TextureLoader(const TextureLoader&);
    TextureLoader& operator=(const TextureLoader&);
    TextureLoader(TextureLoader&&);
    TextureLoader& operator=(TextureLoader&&);

    Texture loadTextureInternal(const std::string& resPath,
        const Texture::Params& params);

    static TextureLoader& get();

private:
    Logger log_;
    std::unordered_map<std::string, TexturePtr> texPathToObject_;
};
} // namespace msgui