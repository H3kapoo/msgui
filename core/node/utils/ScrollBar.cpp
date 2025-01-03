#include "ScrollBar.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"

namespace msgui
{
ScrollBar::ScrollBar(const std::string& name, const ScrollBar::Orientation orientation)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"),
        name, NodeType::SCROLL)
    , log_("ScrollBar(" + name + ")")
    , orientation_(orientation)
{
    knob_ = std::make_shared<ScrollBarKnob>();
    // temporary hardcoded sizes
    knob_->getTransform().setScale({20, 20, 1});
    append(knob_);
}

void ScrollBar::setOverflow(const uint32_t overflow)
{
    overflowSize_ = overflow;
}

void* ScrollBar::getProps()
{
    return &props;
}

float ScrollBar::getKnobOffset()
{
    return knobOffset_;
}

int32_t ScrollBar::geOverflowOffset()
{
    // When knobOffset is really small, the scrollbar will experience a "long range" of knob places
    // in which the value should be "1" for the returned offset but instead it is "0".
    // Current fix is to bump the offset back to "1" if knobOffset is still non zero even if the computed
    // offset turns out to be "0".
    // TODO: Maybe there's a better way?
    int32_t offset = knobOffset_ * (float)overflowSize_;
    return offset == 0 && knobOffset_ > 0 ? 1 : offset;
}

ScrollBar::Orientation ScrollBar::getOrientation()
{
    return orientation_;
}

void ScrollBar::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", props.color);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

void ScrollBar::onMouseButtonNotify()
{
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        return;
    }

    // offset: 0.........1
    // size:  top_xy........bottom_xy

    const int32_t knobHalf = 10;
    // const int32_t knobHalf = 0;
    if (orientation_ == Orientation::VERTICAL)
    {
        knobOffset_ = Utils::remap(state_->mouseY,
            transform_.pos.y + knobHalf, transform_.pos.y + transform_.scale.y - knobHalf, 0.0f, 1.0f);
    }
    else if (orientation_ == Orientation::HORIZONTAL)
    {
        knobOffset_ = Utils::remap(state_->mouseX,
            transform_.pos.x + knobHalf, transform_.pos.x + transform_.scale.x - knobHalf, 0.0f, 1.0f);
    }
    log_.infoLn("clicked %f", knobOffset_);

    state_->isLayoutDirty = true;
}
} // namespace msgui