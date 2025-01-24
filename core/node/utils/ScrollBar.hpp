#pragma once

#include <memory>
#include <string>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/ScrollBarKnob.hpp"

namespace msgui
{
/* Manages scroll actions on an attached Box node */
class ScrollBar : public AbstractNode
{
struct Props;
public:
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL,
        ALL,
        NONE
    };

public:
    ScrollBar(const std::string& name, const Orientation orientation);

    bool setOverflow(const int32_t overflow);
    Props& setColor(const glm::vec4& color);
    Props& setScrollbarSize(const int32_t size);

    glm::vec4 getColor() const;
    int32_t getScrollbarSize() const;
    float getKnobOffset();
    int32_t geOverflowOffset();
    int32_t getOverflowSize();
    Orientation getOrientation();

private:
    void updateKnobOffset();

    void setShaderAttributes() override;

    friend ScrollBarKnob;
    void onMouseButtonNotify() override;
    void onMouseHoverNotify() override;
    void onMouseDragNotify() override;

private:
    struct Props
    {
        glm::vec4 color{Utils::hexToVec4("#ffffffff")};
        int32_t sbSize{20};
    };
    Props props;

    Logger log_;
    float knobOffset_{0};
    int32_t overflowSize_{0};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    Orientation orientation_{Orientation::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
} // namespace msgui