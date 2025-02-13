#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"

namespace msgui
{
/* The moving knob for the Slider */
class SliderKnob : public AbstractNode
{
public:
    SliderKnob(const std::string& name);

    SliderKnob& setColor(const glm::vec4& color);
    SliderKnob& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;

private:
    void setShaderAttributes() override;
    void onMouseDragNotify() override;

private:
    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    Logger log_{"SliderKnob"};
};
using SliderKnobPtr = std::shared_ptr<SliderKnob>;
} // namespace msgui