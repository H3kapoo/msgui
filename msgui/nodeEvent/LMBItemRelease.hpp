#pragma once

#include <cstdint>

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBItemRelease : public INEvent
{
    explicit LMBItemRelease(int32_t indexIn)
        : index{indexIn}
    {}

    int32_t index{0};
};
} // namespace msgui::nodeevent
