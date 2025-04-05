#pragma once

#include "msgui/events/INodeEvent.hpp"

namespace msgui::events
{
struct LMBReleaseNotHovered : public INEvent
{
    explicit LMBReleaseNotHovered()
    {}
};
} // namespace msgui::events
