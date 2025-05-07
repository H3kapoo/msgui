#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace msgui::node::utils
{
struct ListItem
{
    glm::vec4 color;
    std::string text;
};

using ListItemPtr = std::shared_ptr<ListItem>;
using ListItemWPtr = std::weak_ptr<ListItem>;
} // namespace msgui::node::utils
