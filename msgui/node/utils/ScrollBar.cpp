#include "ScrollBar.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/utils/LayoutData.hpp"

namespace msgui
{
ScrollBar::ScrollBar(const std::string& name, const ScrollBar::Type orientation)
    : AbstractNode(name, NodeType::SCROLL)
    , log_("ScrollBar(" + name + ")")
    , orientation_(orientation)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    knob_ = std::make_shared<ScrollBarKnob>();
    layout_.borderRadius = Layout::TBLR{8};
    color_ = Utils::hexToVec4("#eeffccff");

    setScrollbarSize(sbSize_);

    getEvents().listen<nodeevent::LMBClick, InputChannel>(
        std::bind(&ScrollBar::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBClick, InternalChannel>(
        std::bind(&ScrollBar::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBDrag, InputChannel>(
        std::bind(&ScrollBar::onMouseDrag, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBDrag, InternalChannel>(
        std::bind(&ScrollBar::onMouseDrag, this, std::placeholders::_1));
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
    if (orientation_ == Type::VERTICAL)
    {
        knobOffset_ = Utils::remap(getState()->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (orientation_ == Type::HORIZONTAL)
    {
        knobOffset_ = Utils::remap(getState()->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }
}

void ScrollBar::onMouseClick(const nodeevent::LMBClick&)
{
    glm::vec2 knobHalf = glm::vec2{knob_->getTransform().scale.x / 2, knob_->getTransform().scale.y / 2};
    glm::vec2 kPos = knob_->getTransform().pos;

    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        /* Compute distance offset to the knob center for more natural knob dragging behavior. */
        mouseDistFromKnobCenter_.x = getState()->mouseX - (kPos.x + knobHalf.x);
        mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
            ? 0 : mouseDistFromKnobCenter_.x;
        mouseDistFromKnobCenter_.y = getState()->mouseY - (kPos.y + knobHalf.y);
        mouseDistFromKnobCenter_.y = std::abs(mouseDistFromKnobCenter_.y) > knobHalf.y
            ? 0 : mouseDistFromKnobCenter_.y;
    }

    updateKnobOffset();
    MAKE_LAYOUT_DIRTY
}

void ScrollBar::onMouseDrag(const nodeevent::LMBDrag&)
{
    updateKnobOffset();
    MAKE_LAYOUT_DIRTY
}

bool ScrollBar::setOverflow(const int32_t overflow)
{
    if (overflowSize_ == overflow) { return false; }

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
    if (orientation_ == Type::HORIZONTAL)
    {
        transform_.scale.y = sbSize_;
    }
    else if (orientation_ == Type::VERTICAL)
    {
        transform_.scale.x = sbSize_;
    }
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

ScrollBar& ScrollBar::setScrollCurrentValue(const float value)
{
    knobOffset_ = Utils::remap(value, 0, overflowSize_, 0.0f, 1.0f);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

glm::vec4 ScrollBar::getColor() const { return color_; }

int32_t ScrollBar::getScrollbarSize() const { return sbSize_; }

float ScrollBar::getKnobOffset() { return knobOffset_; }

int32_t ScrollBar::geOverflowOffset()
{
    /* When knobOffset is really small, the scrollbar will experience a "long range" of knob places
       in which the value should be "1" for the returned offset but instead it is "0".
       Current fix is to bump the offset back to "1" if knobOffset is still non zero even if the computed
       offset turns out to be "0". */
    // TODO: Maybe there's a better way?
    int32_t offset = knobOffset_ * (float)overflowSize_;
    // return offset == 0 && knobOffset_ > 0 ? 1 : offset;
    return offset;
}

int32_t ScrollBar::getOverflowSize() { return overflowSize_; }

ScrollBar::Type ScrollBar::getOrientation() { return orientation_; }
} // namespace msgui