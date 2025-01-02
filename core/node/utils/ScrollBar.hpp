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
    // Internal Defs
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

public:
    ScrollBar(const std::string& name, const Orientation orientation);

    // Overrides
    void* getProps() override;

    // Getters
    float getOffset();
    Orientation getOrientation();

private:
    // Overrides
    void setShaderAttributes() override;

    // Virtual Event Listeners
    void onMouseButtonNotify() override;

private:
    Logger log_;
    glm::vec4 color_{Utils::hexToVec4("#ffffffff")};
    float offset_{0};
    Orientation orientation_{Orientation::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
} // namespace msgui