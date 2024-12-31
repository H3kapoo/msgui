#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"

namespace msgui
{
/* The knob of the Scrollbar */
class ScrollBarKnob : public AbstractNode
{
public:
    ScrollBarKnob();

    // Overrides
    void* getProps() override;

private:
    // Overrides
    void setShaderAttributes() override;

private:
    Logger log_{"ScrollBarKnob"};
    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
};
using ScrollBarKnobPtr = std::shared_ptr<ScrollBarKnob>;
} // namespace msgui