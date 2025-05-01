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
    Slider(const std::string& name);

    Slider& setColor(const glm::vec4& color);
    Slider& setBorderColor(const glm::vec4& color);
    Slider& setSlideFrom(const float value);
    Slider& setSlideTo(const float value);
    Slider& setSlideCurrentValue(const float value);
    Slider& setSensitivity(const float value);
    Slider& setViewValueFormatPredicate(const std::function<std::string(float)>& pred);
    Slider& enableViewValue(const bool value);
    Slider& enableDynamicKnob(const bool value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    float getSlideFrom() const;
    float getSlideTo() const;
    float getSlideCurrentValue() const;
    int32_t getGirth() const;
    float getOffsetPerc() const;
    bool isDyanmicKnobEnabled() const;
    SliderKnobWPtr getKnob();
    TextLabelWPtr getTextLabel();

private:
    void setShaderAttributes() override;
    void updateSliderValue();
    void updateTextValue();
    void setupLayoutReloadables();

    /* Can't be copied or moved. */
    Slider(const Slider&) = delete;
    Slider(Slider&&) = delete;
    Slider& operator=(const Slider&) = delete;
    Slider& operator=(Slider&&) = delete;

private: // friend
    friend SliderKnob;

    void onMouseWheel(const events::WheelScroll& evt);
    void onMouseClick(const events::LMBClick& evt);
    void onMouseRelease(const events::LMBRelease& evt);
    void onMouseDrag(const events::LMBDrag& evt);

private:
    glm::vec4 color_{Utils::hexToVec4("#ffffffff")};
    glm::vec4 borderColor_{Utils::hexToVec4("#ffffffff")};
    float slideFrom_{0};
    float slideTo_{100};
    float slideValue_{0};
    float sensitivity_{4};
    bool isViewValueEnabled_{true};
    bool dynamicKnobEnabled_{false};
    float knobOffsetPerc_{0};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    std::function<std::string(float)> textViewPred_{nullptr};

    SliderKnobPtr knobNode_{nullptr};
    TextLabelPtr textLabel_{nullptr};
};
using SliderPtr = std::shared_ptr<Slider>;
using SliderWPtr = std::weak_ptr<Slider>;

} // namespace msgui