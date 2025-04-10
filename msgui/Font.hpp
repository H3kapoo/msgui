#pragma once

#include <stdint.h>
#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace msgui
{
static constexpr int32_t MAX_CODEPOINTS    {256};
static constexpr int32_t DEFAULT_FONT_SIZE {16};
static constexpr int32_t MIN_FONT_SIZE     {10};
static constexpr int32_t MAX_FONT_SIZE     {88};
static const std::string DEFAULT_FONT_PATH {"/home/hekapoo/Documents/probe/newgui/assets/fonts/Arial.ttf"};

struct Font
{
    struct CodePointData
    {
        uint32_t charCode;
        int64_t hAdvance;
        glm::ivec2 size;
        glm::ivec2 bearing;
    };

    CodePointData codePointData[MAX_CODEPOINTS];
    uint32_t texId{0};
    int32_t fontSize{16};
    std::string fontPath;
};
using FontPtr = std::shared_ptr<Font>;
} // namespace msgui