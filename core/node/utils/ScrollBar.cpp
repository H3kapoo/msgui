#include "ScrollBar.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
ScrollBar::ScrollBar(const std::string& name, const ScrollBar::Orientation orientation)
    : AbstractNode(name, NodeType::SCROLL)
    , log_("ScrollBar(" + name + ")")
    , orientation_(orientation)
{
    setShader(ShaderLoader::load("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    knob_ = std::make_shared<ScrollBarKnob>();
    layout_.borderRadius = Layout::TBLR{8};
    color_ = Utils::hexToVec4("#eeffccff");

    // Note: Current limitation is that H and V scrollbars will always have the same size.
    // Cannot do independent size based on orientation yet.
    setScrollbarSize(sbSize_);

    append(knob_);
}

void ScrollBar::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void ScrollBar::updateKnobOffset()
{
    glm::vec2 knobHalf = glm::vec2{knob_->getTransform().scale.x / 2, knob_->getTransform().scale.y / 2};
    if (orientation_ == Orientation::VERTICAL)
    {
        knobOffset_ = Utils::remap(getState()->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (orientation_ == Orientation::HORIZONTAL)
    {
        // log_.debugLn("diff is %d", diff);
        knobOffset_ = Utils::remap(getState()->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }
}

void ScrollBar::onMouseButtonNotify()
{
    glm::vec2 knobHalf = glm::vec2{knob_->getTransform().scale.x / 2, knob_->getTransform().scale.y / 2};
    glm::vec2 kPos = knob_->getTransform().pos;

    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // Compute distance offset to the knob center for more natural knob dragging behavior.
        mouseDistFromKnobCenter_.x = getState()->mouseX - (kPos.x + knobHalf.x);
        mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
            ? 0 : mouseDistFromKnobCenter_.x;
        mouseDistFromKnobCenter_.y = getState()->mouseY - (kPos.y + knobHalf.y);
        mouseDistFromKnobCenter_.y = std::abs(mouseDistFromKnobCenter_.y) > knobHalf.y
            ? 0 : mouseDistFromKnobCenter_.y;
    }

    updateKnobOffset();
    getState()->isLayoutDirty = true;
}

void ScrollBar::onMouseHoverNotify()
{}

void ScrollBar::onMouseDragNotify()
{
    updateKnobOffset();
    getState()->isLayoutDirty = true;
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

ScrollBar& ScrollBar::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

ScrollBar& ScrollBar::setScrollbarSize(const int32_t size)
{
    sbSize_ = size;
    if (orientation_ == Orientation::HORIZONTAL)
    {
        transform_.scale.y = sbSize_;
    }
    else if (orientation_ == Orientation::VERTICAL)
    {
        transform_.scale.x = sbSize_;
    }
    return *this;
}

glm::vec4 ScrollBar::getColor() const { return color_; }

int32_t ScrollBar::getScrollbarSize() const { return sbSize_; }


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

} // namespace msgui