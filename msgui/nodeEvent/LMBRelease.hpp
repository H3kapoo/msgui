#pragma once

#include <glm/ext/vector_int2.hpp>

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBRelease : public INEvent
{
    explicit LMBRelease(const glm::ivec2& posIn)
        : pos{posIn}
    {}

    glm::ivec2 pos;
};
} // namespace msgui::nodeevent
