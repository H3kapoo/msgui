#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>

namespace msgui
{
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

    explicit Texture(const uint32_t id, const uint32_t width, const uint32_t height, const uint32_t numChannels,
        const Params& params);

    // Getters
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