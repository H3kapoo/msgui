#pragma once

#include <memory>
#include <string>

#include "msgui/node/AbstractNode.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/utils/ScrollBarKnob.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"

namespace msgui
{
/* Manages scroll actions on an attached Box node */
class ScrollBar : public AbstractNode
{
public:
    enum class Type
    {
        HORIZONTAL,
        VERTICAL,
        ALL,
        NONE
    };

public:
    ScrollBar(const std::string& name, const Type orientation);

    bool setOverflow(const int32_t overflow);
    ScrollBar& setColor(const glm::vec4& color);
    ScrollBar& setScrollbarSize(const int32_t size);
    ScrollBar& setScrollCurrentValue(const float value);

    glm::vec4 getColor() const;
    int32_t getScrollbarSize() const;
    float getKnobOffset();
    int32_t geOverflowOffset();
    int32_t getOverflowSize();
    Type getOrientation();

private:
    ScrollBar(const ScrollBar&) = delete;
    ScrollBar(ScrollBar&&) = delete;
    ScrollBar& operator=(const ScrollBar&) = delete;
    ScrollBar& operator=(ScrollBar&&) = delete;

    void setShaderAttributes() override;
    void updateKnobOffset();

    friend ScrollBarKnob;
    void onMouseClick(const events::LMBClick& evt);
    void onMouseDrag(const events::LMBDrag& evt);

private:
    Logger log_;
    glm::vec4 color_{Utils::hexToVec4("#ffffffff")};
    int32_t sbSize_{20};
    float knobOffset_{0};
    int32_t overflowSize_{0};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    Type orientation_{Type::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
using ScrollBarWPtr = std::weak_ptr<ScrollBar>;
} // namespace msgui