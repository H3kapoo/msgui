#include "TextureLoader.hpp"
#include "Logger.hpp"
#include "core/Window.hpp"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

namespace msgui
{
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
        instance.log_.errorLn("Load failed! Returning zero texture!");
    }

    return texturePtr;
}

Texture TextureLoader::loadTextureInternal(const std::string& resPath,
    const Texture::Params& params)
{
    log_ = Logger("TextureLoader(" + resPath + ")");

    /* Details:
        When a new thread is created and we try to load a texture from inside it, operation will fail and
        return garbage texture id.
        This is because glfw contexts are thread based so even if in the main thread we had a context to 
        work with (sharedWindowHandle_), in this new thread we dont have one to work with.
       Current solution:
        Solution for now is to create a new "dummy" hidden window and make it share it's context with the
        main thread's contex (sharedWindowHandle_). That way whatever this new thread loads in in terms
        of textures, the main thread will know and make use of.
        We can safely get rid of this new window afterwards.
       Concerns:
        - Static windowHandle inside here ain't optimal but we need it.

    */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    static GLFWwindow* windowHandle = glfwCreateWindow(2, 2, "dummy", NULL, Window::getSharedContexWindowHandle());
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwMakeContextCurrent(windowHandle);

    uint32_t id;
    int32_t width, height, numChannels;
    unsigned char* data = stbi_load(resPath.c_str(), &width, &height, &numChannels, 0);
    if (!data)
    {
        log_.errorLn("Cannot load texture. Check path correctness!");
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

    /* TODO: or 1D/3D.. to be adapted later */
    glTexImage2D(params.target, 0, glColorFormat, width, height, 0, glColorFormat,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(params.target);

    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(windowHandle);

    stbi_image_free(data);

    return Texture(id, width, height, numChannels, params);
}

TextureLoader& TextureLoader::get()
{
    static TextureLoader instance;
    return instance;
}
} // msgui