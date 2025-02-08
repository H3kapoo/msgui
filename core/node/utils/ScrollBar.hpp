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
    ScrollBar& setColor(const glm::vec4& color);
    ScrollBar& setScrollbarSize(const int32_t size);
    ScrollBar& setScrollCurrentValue(const float value);

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
    Logger log_;
    glm::vec4 color_{Utils::hexToVec4("#ffffffff")};
    int32_t sbSize_{20};
    float knobOffset_{0};
    int32_t overflowSize_{0};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    Orientation orientation_{Orientation::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
} // namespace msgui