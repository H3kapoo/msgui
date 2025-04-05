#pragma once

#include <glm/ext/vector_int2.hpp>

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBRelease : public INEvent
{
    explicit LMBRelease(const glm::ivec2& posIn)
        : pos{posIn}
    {}

    glm::ivec2 pos;
};
} // namespace msgui::events
