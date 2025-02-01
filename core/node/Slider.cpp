#include "Slider.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/SliderKnob.hpp"

namespace msgui
{
Slider::Slider(const std::string& name) : AbstractNode(name, NodeType::SLIDER)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    knobNode_ = std::make_shared<SliderKnob>("Knob");
    knobNode_->setColor(Utils::hexToVec4("#ee0000ff"));
    // knobNode_->getTransform().scale = glm::vec3(50, 50, 1);
    knobNode_->getLayout().setScale({50, 50});
    append(knobNode_);

    setupLayoutReloadables();
}

void Slider::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Slider::updateSliderValue()
{
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    if (layout_.type == Layout::Type::VERTICAL)
    {
        knobOffsetPerc_ = Utils::remap(getState()->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (layout_.type == Layout::Type::HORIZONTAL)
    {
        knobOffsetPerc_ = Utils::remap(getState()->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }

    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);

    listeners_.callOnSlide(slideValue_);
}

void Slider::onMouseButtonNotify()
{
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    glm::vec2 kPos = knobNode_->getTransform().pos;
    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // Compute distance offset to the knob center for more natural knob dragging behavior.
        mouseDistFromKnobCenter_.x = getState()->mouseX - (kPos.x + knobHalf.x);
        mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
            ? 0 : mouseDistFromKnobCenter_.x;
        mouseDistFromKnobCenter_.y = getState()->mouseY - (kPos.y + knobHalf.y);
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

void Slider::setupLayoutReloadables()
{
    // props.orientType.onReload = [this]()
    // {
    //     std::swap(transform_.scale.x, transform_.scale.y);
    //     MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    // };
}

Slider& Slider::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

Slider& Slider::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

Slider& Slider::setSlideFrom(const float value)
{
    slideFrom_ = value;
    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Slider& Slider::setSlideTo(const float value)
{
    slideTo_ = value;
    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Slider& Slider::setSlideCurrentValue(const float value)
{
    slideValue_ = value;
    knobOffsetPerc_ = Utils::remap(slideValue_, slideFrom_, slideTo_, 0.0f, 1.0f);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

SliderListeners& Slider::getListeners() { return listeners_; }


glm::vec4 Slider::getColor() const { return color_; }

glm::vec4 Slider::getBorderColor() const { return borderColor_; }

float Slider::getSlideFrom() const { return slideFrom_; }

float Slider::getSlideTo() const { return slideTo_; }

float Slider::getSlideCurrentValue() const { return slideValue_; }

SliderKnobPtr Slider::getKnobRef() {return knobNode_; }

float Slider::getOffsetPerc() const { return knobOffsetPerc_; }
} // msgui