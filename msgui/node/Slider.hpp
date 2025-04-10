#pragma once

#include <memory>

#include "msgui/events/WheelScroll.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/utils/SliderKnob.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"

namespace msgui
{
/* Class for handling sliding values on a bar. */
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
    Slider& setSensitivity(const float value);
    Slider& enableViewValue(const bool value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    float getSlideFrom() const;
    float getSlideTo() const;
    float getSlideCurrentValue() const;
    int32_t getGirth() const;
    float getOffsetPerc() const;
    SliderKnobWPtr getKnob();
    TextLabelWPtr getTextLabel();

private:
    void setShaderAttributes() override;
    void updateSliderValue();
    void updateTextValue();
    void setupLayoutReloadables();

private: // friend
    friend SliderKnob;

    void onMouseWheel(const events::WheelScroll& evt);
    void onMouseClick(const events::LMBClick& evt);
    void onMouseRelease(const events::LMBRelease& evt);
    void onMouseDrag(const events::LMBDrag& evt);

private:
    Logger log_{"Slider"};

    glm::vec4 color_{Utils::hexToVec4("#000000ff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ff0000ff")};
    float slideFrom_{0};
    float slideTo_{0};
    float slideValue_{0};
    float sensitivity_{1};
    bool isViewValueEnabled_{true};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    float knobOffsetPerc_{0};
    SliderKnobPtr knobNode_{nullptr};
    TextLabelPtr textLabel_{nullptr};
};
using SliderPtr = std::shared_ptr<Slider>;
using SliderWPtr = std::weak_ptr<Slider>;
} // namespace msgui