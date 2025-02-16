#pragma once

#include <memory>

#include "msgui/Listeners.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/utils/SliderKnob.hpp"

namespace msgui
{
/* Sliding value node class */
class Slider : public AbstractNode
{
public:
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

public:
    Slider(const std::string& name);

    Slider& setColor(const glm::vec4& color);
    Slider& setBorderColor(const glm::vec4& color);
    Slider& setSlideFrom(const float value);
    Slider& setSlideTo(const float value);
    Slider& setSlideCurrentValue(const float value);
    Slider& setGirth(const int32_t value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    float getSlideFrom() const;
    float getSlideTo() const;
    float getSlideCurrentValue() const;
    int32_t getGirth() const;
    SliderListeners& getListeners();
    SliderKnobPtr getKnobRef();
    float getOffsetPerc() const;

private:
    void setShaderAttributes() override;
    void updateSliderValue();
    void setupLayoutReloadables();

private: // friend
    friend SliderKnob;
    void onMouseDragNotify() override;

private:
    Logger log_{"Slider"};

    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    float slideFrom_{0};
    float slideTo_{0};
    float slideValue_{0};
    SliderListeners listeners_;
    glm::ivec2 mouseDistFromKnobCenter_{0};
    float knobOffsetPerc_{0};
    SliderKnobPtr knobNode_{nullptr};
};
using SliderPtr = std::shared_ptr<Slider>;
} // namespace msgui