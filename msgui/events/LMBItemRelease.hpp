#pragma once

#include <cstdint>

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBItemRelease : public INEvent
{
    explicit LMBItemRelease(int32_t indexIn)
        : index{indexIn}
    {}

    int32_t index{0};
};
} // namespace msgui::events
