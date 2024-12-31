#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
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
    ScrollBar(const Orientation orientation);

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
    Logger log_{"ScrollBar"};
    glm::vec4 color_{Utils::hexToVec4("#ffffffff")};
    float offset_{0};
    Orientation orientation_{Orientation::VERTICAL};
    ScrollBarKnobPtr knob_{nullptr};
};
using ScrollBarPtr = std::shared_ptr<ScrollBar>;
} // namespace msgui