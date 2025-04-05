#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"

namespace msgui
{
/* The knob of the Scrollbar */
class ScrollBarKnob : public AbstractNode
{
public:
    ScrollBarKnob();

    ScrollBarKnob& setColor(const glm::vec4& color);
    ScrollBarKnob& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;

private:
    ScrollBarKnob(const ScrollBarKnob&) = delete;
    ScrollBarKnob(ScrollBarKnob&&) = delete;
    ScrollBarKnob& operator=(const ScrollBarKnob&) = delete;
    ScrollBarKnob& operator=(ScrollBarKnob&&) = delete;

    void setShaderAttributes() override;

    void onMouseClick(const events::LMBClick& evt);
    void onMouseDrag(const events::LMBDrag& evt);

private:
    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    Logger log_{"ScrollBarKnob"};
};
using ScrollBarKnobPtr = std::shared_ptr<ScrollBarKnob>;
} // namespace msgui