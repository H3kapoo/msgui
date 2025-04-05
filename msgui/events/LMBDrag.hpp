#pragma once

#include <cstdint>

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBDrag : public INEvent
{
    explicit LMBDrag(int32_t xIn, int32_t yIn)
        : x{xIn}
        , y{yIn}
    {}

    int32_t x{0};
    int32_t y{0};
};
} // namespace msgui::events
