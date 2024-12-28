#include "TextureLoader.hpp"
#include "Logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

namespace msgui
{
TextureLoader& TextureLoader::get()
{
    static TextureLoader instance;
    return instance;
}

TexturePtr TextureLoader::loadTexture(const std::string& resPath,
    const Texture::Params& params)
{
    TextureLoader& instance = get();
    if (instance.texPathToObject_.contains(resPath))
    {
        return instance.texPathToObject_.at(resPath);
    }

    TexturePtr texturePtr = std::make_shared<Texture>(instance.loadTextureInternal(resPath, params));
    if (texturePtr->getId() != 0)
    {
        instance.texPathToObject_[resPath] = texturePtr;
        instance.log_.infoLn("Loaded! ( %dx%dx%d )", texturePtr->getWidth(),texturePtr->getHeight(),
            texturePtr->getNumChannels());
    }
    else
    {
        instance.log_.error("Load failed! Returning zero texture!");
    }

    return texturePtr;
}

Texture TextureLoader::loadTextureInternal(const std::string& resPath,
    const Texture::Params& params)
{
    log_ = Logger("TextureLoader(" + resPath + ")");

    uint32_t id;
    int32_t width, height, numChannels;
    unsigned char* data = stbi_load(resPath.c_str(), &width, &height, &numChannels, 0);
    if (!data)
    {
        log_.error("Cannot load texture. Check path correctness!");
        return Texture(0, -1, -1, -1, params);
    }

    uint32_t glColorFormat = resPath.ends_with(".png") ? GL_RGBA : GL_RGB;
    float borderColor[] = {params.borderColor.r, params.borderColor.g, params.borderColor.b, params.borderColor.a};

    glGenTextures(1, &id);
    glBindTexture(params.target, id);
    glTexParameteri(params.target, GL_TEXTURE_WRAP_S, params.uWrap);
    glTexParameteri(params.target, GL_TEXTURE_WRAP_T, params.vWrap);
    glTexParameteri(params.target, GL_TEXTURE_MIN_FILTER, params.minFilter);
    glTexParameteri(params.target, GL_TEXTURE_MAG_FILTER, params.magFilter);
    glTexParameterfv(params.target, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameterf(params.target, GL_TEXTURE_MAX_ANISOTROPY, params.anisotropicFiltering);

    // or 1D/3D.. to be adapted later
    glTexImage2D(params.target, 0, glColorFormat, width, height, 0, glColorFormat,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(params.target);

    stbi_image_free(data);

    return Texture(id, width, height, numChannels, params);
}

} // msgui