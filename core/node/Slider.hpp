#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/SliderKnob.hpp"

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

    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{Utils::hexToVec4("#000000ff")};
        glm::vec4 borderColor{Utils::hexToVec4("#ff0000ff")};
        Orientation orientation{Orientation::HORIZONTAL};
        AR<float> slideFrom{0};
        AR<float> slideTo{0};
        AR<float> slideValue{0};
        // float steps{0}; // Snapping TBA
    };

public:
    Slider(const std::string& name);

    void* getProps() override;

    float getOffsetPerc() const;

private:
    void setShaderAttributes() override;

    void updateSliderValue();

    friend SliderKnob;
    void onMouseButtonNotify() override;
    void onMouseHoverNotify() override;
    void onMouseDragNotify() override;

    void setupReloadables();

public:
    Props props;

private:
    Logger log_{"Slider"};
    glm::ivec2 mouseDistFromKnobCenter_{0};
    float knobOffsetPerc_{0};
    SliderKnobPtr knobNode_{nullptr};
};
using SliderPtr = std::shared_ptr<Slider>;
} // namespace msgui