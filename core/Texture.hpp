#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>

namespace msgui
{

/* Holder for texture related information */
struct Texture
{
    struct Params
    {
        uint32_t target            {GL_TEXTURE_2D};
        uint32_t uWrap             {GL_REPEAT};
        uint32_t vWrap             {GL_REPEAT};
        uint32_t minFilter         {GL_LINEAR_MIPMAP_LINEAR};
        uint32_t magFilter         {GL_LINEAR};
        glm::vec4 borderColor      {1.0f};
        float anisotropicFiltering {8.0f};
    };

    /**
        Creates a new texture structure. To be used by texture loader.

        @param id Loaded texture id
        @param width Texture width
        @param height Texture height
        @param numChannels Texture channels (3 for JPG, 4 for PNG, etc)
        @param params Texture configuration params
    */
    explicit Texture(const uint32_t id, const uint32_t width, const uint32_t height, const uint32_t numChannels,
        const Params& params);

    uint32_t getId() const;
    int32_t getWidth() const;
    int32_t getHeight() const;
    int32_t getNumChannels() const;
    const Params& getParams() const;

private:
    uint32_t id_{0};
    int32_t width_{-1};
    int32_t height_{-1};
    int32_t numChannels_{-1};
    Params params_;
};

using TexturePtr = std::shared_ptr<Texture>;
} // namespace msgui