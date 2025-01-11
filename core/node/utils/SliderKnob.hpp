#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
/* The moving knob for the Slider */
class SliderKnob : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{Utils::hexToVec4("#000000ff")};
        glm::vec4 borderColor{Utils::hexToVec4("#ff0000ff")};
    };

public:
    SliderKnob(const std::string& name);

    void* getProps() override;

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;
    void onMouseHoverNotify() override;
    void onMouseDragNotify() override;

public:
    Props props;

private:
    Logger log_{"SliderKnob"};
};
using SliderKnobPtr = std::shared_ptr<SliderKnob>;
} // namespace msgui