#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
/* The knob of the Scrollbar */
class ScrollBarKnob : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{Utils::hexToVec4("#000000ff")};
        glm::vec4 borderColor{Utils::hexToVec4("#ff0000ff")};
    };

public:
    ScrollBarKnob();

    void* getProps() override;

private:
    void setShaderAttributes() override;

public:
    Props props;

private:
    Logger log_{"ScrollBarKnob"};
};
using ScrollBarKnobPtr = std::shared_ptr<ScrollBarKnob>;
} // namespace msgui