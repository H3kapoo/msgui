#pragma once

#include <optional>
#include <string>
#include <vector>
#include <list>

#include <glm/glm.hpp>

#include "msgui/layoutEngine/utils/Transform.hpp"
#include "msgui/Font.hpp"

namespace msgui::renderer
{
using namespace msgui::layoutengine;

struct PerCodepointData
{
    std::vector<glm::mat4> transform;
    std::vector<int32_t> unicodeIndex;
};

struct TextData
{
    std::string text;
    glm::vec4 color{1.0f};
    bool isDirty{true};
    PerCodepointData pcd;
    FontPtr fontData{nullptr};
    utils::Transform* transformPtr{nullptr};
    glm::ivec2 textBounds{0, 0};
    // other data
};

using TextDataList = std::list<TextData>;
using TextDataListIt = TextDataList::iterator;
using MaybeTextDataIt = std::optional<TextDataListIt>;
} // namespace msgui::renderer