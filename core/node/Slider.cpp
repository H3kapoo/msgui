#include "Slider.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/SliderKnob.hpp"

namespace msgui
{
Slider::Slider(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::SLIDER)
{
    knobNode_ = std::make_shared<SliderKnob>("Knob");
    knobNode_->props.color = Utils::hexToVec4("#ee0000ff");
    knobNode_->getTransform().scale = glm::vec3(50, 50, 1);
    append(knobNode_);

    setupReloadables();
}

void* Slider::getProps()
{
    return &props;
}

SliderKnobPtr Slider::getKnobRef()
{
    return knobNode_;
}

float Slider::getOffsetPerc() const
{
    return knobOffsetPerc_;
}

void Slider::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border.value);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius.value);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Slider::updateSliderValue()
{
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    if (props.orientType == Layout::Type::VERTICAL)
    {
        knobOffsetPerc_ = Utils::remap(state_->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (props.orientType == Layout::Type::HORIZONTAL)
    {
        knobOffsetPerc_ = Utils::remap(state_->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }

    props.slideValue.value = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, props.slideFrom, props.slideTo);
    listeners.callOnSlide(props.slideValue.value);
}

void Slider::onMouseButtonNotify()
{
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    glm::vec2 kPos = knobNode_->getTransform().pos;
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // Compute distance offset to the knob center for more natural knob dragging behavior.
        mouseDistFromKnobCenter_.x = state_->mouseX - (kPos.x + knobHalf.x);
        mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
            ? 0 : mouseDistFromKnobCenter_.x;
        mouseDistFromKnobCenter_.y = state_->mouseY - (kPos.y + knobHalf.y);
        mouseDistFromKnobCenter_.y = std::abs(mouseDistFromKnobCenter_.y) > knobHalf.y
            ? 0 : mouseDistFromKnobCenter_.y;

        updateSliderValue();
        MAKE_LAYOUT_DIRTY
    }
}

void Slider::onMouseHoverNotify()
{}

void Slider::onMouseDragNotify()
{
    updateSliderValue();
    MAKE_LAYOUT_DIRTY
}

void Slider::setupReloadables()
{
    props.orientType.onReload = [this]()
    {
        std::swap(transform_.scale.x, transform_.scale.y);
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    };

    props.slideValue.onReload = [this]()
    {
        knobOffsetPerc_ = Utils::remap(props.slideValue.value, props.slideFrom, props.slideTo, 0.0f, 1.0f);
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    };

    auto updateSlideValue = [this]()
    {
        props.slideValue.value = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, props.slideFrom, props.slideTo);
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    };

    props.slideFrom.onReload = updateSlideValue;
    props.slideTo.onReload = updateSlideValue;
}

} // msgui