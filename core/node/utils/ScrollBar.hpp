#pragma once

#include <memory>
#include <string>

#include "core/node/AbstractNode.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/LayoutData.hpp"
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
        ALL
    };

    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{Utils::hexToVec4("#ffffffff")};
    };

public:
    ScrollBar(const std::string& name, const Orientation orientation);

    void* getProps() override;

    float getOffset();
    Orientation getOrientation();

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;

public:
    Props props;

private:
    Logger log_;
    float offset_{0};
    Orientation orientation_{Orientation::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
} // namespace msgui