#include "Texture.hpp"

namespace msgui
{
Texture::Texture(const uint32_t id, const uint32_t width, const uint32_t height, const uint32_t numChannels,
    const Params& params)
    : id_(id)
    , width_(width)
    , height_(height)
    , numChannels_(numChannels)
    , params_(params)
{}

uint32_t Texture::getId() const
{
    return id_;
}

int32_t Texture::getWidth() const
{
    return width_;
}

int32_t Texture::getHeight() const
{
    return height_;
}

int32_t Texture::getNumChannels() const
{
    return numChannels_;
}

const Texture::Params& Texture::getParams() const
{
    return params_;
}
} // msgui