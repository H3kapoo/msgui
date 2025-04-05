#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"

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
    void onMouseClick(const events::LMBClick& evt);
    void onMouseDrag(const events::LMBDrag& evt);

    void setupLayoutReloadables();

private:
    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    Logger log_{"SliderKnob"};
};
using SliderKnobPtr = std::shared_ptr<SliderKnob>;
using SliderKnobWPtr = std::weak_ptr<SliderKnob>;
} // namespace msgui