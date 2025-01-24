#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"

namespace msgui
{
/* The knob of the Scrollbar */
class ScrollBarKnob : public AbstractNode
{
struct Props;
public:
    ScrollBarKnob();

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;
    void onMouseHoverNotify() override;
    void onMouseDragNotify() override;


private:
    struct Props
    {
        glm::vec4 color{Utils::hexToVec4("#000000ff")};
        glm::vec4 borderColor{Utils::hexToVec4("#ff0000ff")};
    };
    Props props;
    Logger log_{"ScrollBarKnob"};
};
using ScrollBarKnobPtr = std::shared_ptr<ScrollBarKnob>;
} // namespace msgui