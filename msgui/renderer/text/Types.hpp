#pragma once

#include "msgui/Transform.hpp"
#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace msgui::renderer::text
{
struct PerCodepointData
{
    std::vector<glm::mat4> transform;
    std::vector<int32_t> unicodeIndex;
};

struct TextData
{
    std::string text;
    Transform* transformPtr;
    glm::vec4 color{1.0f};
    bool isDirty{true};
    PerCodepointData pcd;
    // other data
};

using TextDataList = std::vector<TextData>;
using TextDataListIt = TextDataList::iterator;
using MaybeTextDataIt = std::optional<TextDataListIt>;
} // namespace msgui::renderer::text