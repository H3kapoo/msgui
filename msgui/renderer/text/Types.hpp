#pragma once

#include <optional>
#include <string>
#include <vector>
#include <list>
#include <memory>

#include <glm/glm.hpp>

#include "msgui/Transform.hpp"

namespace msgui::renderer::text
{
static constexpr int32_t MAX_CODEPOINTS{256};
static constexpr int32_t DEFAULT_FONT_SIZE{16};
static constexpr int32_t MIN_FONT_SIZE{10};
static constexpr int32_t MAX_FONT_SIZE{88};
static const std::string DEFAULT_FONT_PATH{"/home/hekapoo/Documents/probe/newgui/assets/fonts/Arial.ttf"};

struct CodePointData
{
    uint32_t charCode;
    int64_t hAdvance;
    glm::ivec2 size;
    glm::ivec2 bearing;
};

struct Font
{
    CodePointData codePointData[MAX_CODEPOINTS];
    uint32_t texId{0};
    int32_t fontSize{16};
    std::string fontPath;
};
using FontPtr = std::shared_ptr<Font>;

struct PerCodepointData
{
    std::vector<glm::mat4> transform;
    std::vector<int32_t> unicodeIndex;
};

struct TextData
{
    std::string text;
    Transform* transformPtr{nullptr};
    glm::vec4 color{1.0f};
    bool isDirty{true};
    PerCodepointData pcd;
    FontPtr fontData{nullptr};
    // other data
};

using TextDataList = std::list<TextData>;
using TextDataListIt = TextDataList::iterator;
using MaybeTextDataIt = std::optional<TextDataListIt>;
} // namespace msgui::renderer::text