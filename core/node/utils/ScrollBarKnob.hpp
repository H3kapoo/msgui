#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/nodeEvent/LMBClick.hpp"
#include "core/nodeEvent/LMBDrag.hpp"

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
    void setShaderAttributes() override;

    void onMouseClick(const nodeevent::LMBClick& evt);
    void onMouseDrag(const nodeevent::LMBDrag& evt);

private:
    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    Logger log_{"ScrollBarKnob"};
};
using ScrollBarKnobPtr = std::shared_ptr<ScrollBarKnob>;
} // namespace msgui