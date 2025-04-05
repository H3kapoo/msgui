#include <future>

#define STB_IMAGE_IMPLEMENTATION
#include "msgui/loaders/BELoadingQueue.hpp"
#include "msgui/loaders/TextureLoader.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Texture.hpp"
#include "msgui/vendor/stb_image.h"

namespace msgui::loaders
{
TexturePtr TextureLoader::loadTexture(const std::string& resPath,
    const Texture::Params& params)
{
    TextureLoader& instance = get();
    if (instance.texPathToObject_.contains(resPath))
    {
        return instance.texPathToObject_.at(resPath);
    }

    /* Textures will always be loaded from the main (UI) thread as to not break opengl internals. */
    TexturePtr texturePtr = std::make_shared<Texture>(instance.loadTextureInternal(resPath, params));
    if (texturePtr->getId() != 0)
    {
        instance.texPathToObject_[resPath] = texturePtr;
        instance.log_.infoLn("Loaded! ( %dx%dx%d  id: %u)", texturePtr->getWidth(),texturePtr->getHeight(),
            texturePtr->getNumChannels(), texturePtr->getId());
    }
    else
    {
        instance.log_.errorLn("Load failed! Returning zero texture!");
    }
    return TextureLoader::loadTexture(resPath, params);
}

Texture TextureLoader::loadTextureInternal(const std::string& resPath,
    const Texture::Params& params)
{
    log_ = Logger("TextureLoader(" + resPath + ")");

    int32_t width, height, numChannels;
    unsigned char* data = stbi_load(resPath.c_str(), &width, &height, &numChannels, 0);
    if (!data)
    {
        log_.errorLn("Cannot load texture. Check path correctness!");
        return Texture(-1, 0, 0, 0, params);
    }

    std::packaged_task<uint32_t()> task([this, resPath, params, data, width, height]() -> uint32_t
    {
        uint32_t id;
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

        /* TODO: or 1D/3D.. to be adapted later */
        glTexImage2D(params.target, 0, glColorFormat, width, height, 0, glColorFormat,
            GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(params.target);

        return id;
    });

    auto futureTask = task.get_future();

    if (loaders::BELoadingQueue::get().isThisMainThread()) { task(); }
    /* This is not the main thread */
    else { loaders::BELoadingQueue::get().pushTask(std::move(task)); }

    uint32_t id = futureTask.get();
    stbi_image_free(data);

    return Texture(id, width, height, numChannels, params);
}

TextureLoader& TextureLoader::get()
{
    static TextureLoader instance;
    return instance;
}
} // msgui::loaders