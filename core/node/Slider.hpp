#pragma once

#include <memory>

#include "core/Listeners.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/SliderKnob.hpp"

namespace msgui
{
/* Sliding value node class */
class Slider : public AbstractNode
{
struct Props;
public:
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

public:
    Slider(const std::string& name);

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);
    Props& setSlideFrom(const float value);
    Props& setSlideTo(const float value);
    Props& setSlideCurrentValue(const float value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    float getSlideFrom() const;
    float getSlideTo() const;
    float getSlideCurrentValue() const;
    SliderKnobPtr getKnobRef();
    float getOffsetPerc() const;

private:
    void setShaderAttributes() override;
    void updateSliderValue();
    void setupLayoutReloadables();

private: // friend
    friend SliderKnob;
    void onMouseButtonNotify() override;
    void onMouseHoverNotify() override;
    void onMouseDragNotify() override;

public:
    Listeners listeners;

private:
    struct Props
    {
        glm::vec4 color{Utils::hexToVec4("#000000ff")};
        glm::vec4 borderColor{Utils::hexToVec4("#ff0000ff")};
        float slideFrom{0};
        float slideTo{0};
        float slideValue{0};
        // float steps{0}; // Snapping TBA
    };
    Props props;
    Logger log_{"Slider"};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    float knobOffsetPerc_{0};
    SliderKnobPtr knobNode_{nullptr};
};
using SliderPtr = std::shared_ptr<Slider>;
} // namespace msgui