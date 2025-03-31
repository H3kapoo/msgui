#pragma once

#include <string>
#include <list>
#include <vector>

#include <glm/glm.hpp>

namespace msgui::renderer::text
{
struct TextData
{
    struct PerCodepointData
    {
        std::vector<glm::mat4> transform;
        std::vector<int32_t> unicodeIndex;
    };

    std::string text;
    glm::ivec3 pos{0};
    glm::vec4 color{1.0f};
    PerCodepointData pcd;
    // other data
};
using TextDataList = std::list<TextData>;
using TextDataListIt = std::list<TextData>::iterator;
} // namespace msgui::renderer::text