#pragma once

#include "msgui/nodeEvent/INodeEvent.hpp"

namespace msgui::nodeevent
{
struct LMBReleaseNotHovered : public INEvent
{
    explicit LMBReleaseNotHovered()
    {}
};
} // namespace msgui::nodeevent
