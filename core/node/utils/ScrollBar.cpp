#include "ScrollBar.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
ScrollBar::ScrollBar(const std::string& name, const ScrollBar::Orientation orientation)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::SCROLL)
    , log_("ScrollBar(" + name + ")")
    , orientation_(orientation)
{
    knob_ = std::make_shared<ScrollBarKnob>();
    props.layout.borderRadius = Layout::TBLR{8};
    props.color = Utils::hexToVec4("#eeffccff");

    // Note: Current limitation is that H and V scrollbars will always have the same size.
    // Cannot do independent size based on orientation yet.
    props.sbSize.onReload = [this, orientation]()
    {
        if (orientation == Orientation::HORIZONTAL)
        {
            transform_.scale.y = props.sbSize.value;
        }
        else if (orientation == Orientation::VERTICAL)
        {
            transform_.scale.x = props.sbSize.value;
        }
    };

    props.sbSize.onReload();

    append(knob_);
}

bool ScrollBar::setOverflow(const int32_t overflow)
{
    // We shall indicate if the value was modified or not
    if (overflowSize_ == overflow)
    {
        return false;
    }

    overflowSize_ = overflow;
    return true;
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

int32_t ScrollBar::getOverflowSize()
{
    return overflowSize_;
}

ScrollBar::Orientation ScrollBar::getOrientation()
{
    return orientation_;
}

void ScrollBar::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    // shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void ScrollBar::updateKnobOffset()
{
    glm::vec2 knobHalf = glm::vec2{knob_->getTransform().scale.x / 2, knob_->getTransform().scale.y / 2};
    if (orientation_ == Orientation::VERTICAL)
    {
        knobOffset_ = Utils::remap(state_->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (orientation_ == Orientation::HORIZONTAL)
    {
        // log_.debugLn("diff is %d", diff);
        knobOffset_ = Utils::remap(state_->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }
}

void ScrollBar::onMouseButtonNotify()
{
    glm::vec2 knobHalf = glm::vec2{knob_->getTransform().scale.x / 2, knob_->getTransform().scale.y / 2};
    glm::vec2 kPos = knob_->getTransform().pos;

    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // Compute distance offset to the knob center for more natural knob dragging behavior.
        mouseDistFromKnobCenter_.x = state_->mouseX - (kPos.x + knobHalf.x);
        mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
            ? 0 : mouseDistFromKnobCenter_.x;
        mouseDistFromKnobCenter_.y = state_->mouseY - (kPos.y + knobHalf.y);
        mouseDistFromKnobCenter_.y = std::abs(mouseDistFromKnobCenter_.y) > knobHalf.y
            ? 0 : mouseDistFromKnobCenter_.y;
    }

    updateKnobOffset();
    state_->isLayoutDirty = true;
}

void ScrollBar::onMouseHoverNotify()
{}

void ScrollBar::onMouseDragNotify()
{
    updateKnobOffset();
    state_->isLayoutDirty = true;
}
} // namespace msgui