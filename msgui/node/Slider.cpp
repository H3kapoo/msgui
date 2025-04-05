#include "Slider.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/utils/SliderKnob.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/events/Scroll.hpp"

namespace msgui
{
Slider::Slider(const std::string& name) : AbstractNode(name, NodeType::SLIDER)
{
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    knobNode_ = std::make_shared<SliderKnob>("Knob");
    knobNode_->setColor(Utils::hexToVec4("#ee0000ff"));

    /* Assume default horizontal slider. */
    knobNode_->getLayout()
        .setScale({20, 1.0f})
        .setScaleType({utils::Layout::ScaleType::PX, utils::Layout::ScaleType::REL});

    append(knobNode_);

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setScale({200, 34});

    /* Register only the events you need. */
    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&Slider::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InputChannel>(
        std::bind(&Slider::onMouseDrag, this, std::placeholders::_1));
    getEvents().listen<events::LMBClick, events::InternalChannel>(
        std::bind(&Slider::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InternalChannel>(
        std::bind(&Slider::onMouseDrag, this, std::placeholders::_1));
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
    if (layout_.type == utils::Layout::Type::VERTICAL)
    {
        knobOffsetPerc_ = Utils::remap(getState()->mouseY - mouseDistFromKnobCenter_.y,
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, 0.0f, 1.0f);
    }
    else if (layout_.type == utils::Layout::Type::HORIZONTAL)
    {
        knobOffsetPerc_ = Utils::remap(getState()->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, 0.0f, 1.0f);
    }

    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);

    events::Scroll evt{slideValue_};
    getEvents().notifyAllChannels<events::Scroll>(evt);
}

void Slider::onMouseClick(const events::LMBClick&)
{
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    glm::vec2 kPos = knobNode_->getTransform().pos;
    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        /* Compute distance offset to the knob center for more natural knob dragging behavior. */
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

void Slider::onMouseDrag(const events::LMBDrag&)
{
    updateSliderValue();
    MAKE_LAYOUT_DIRTY
}

void Slider::setupLayoutReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME; };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
    layout_.onTypeChange = [this]()
    {
        std::swap(knobNode_->getLayout().scale.x, knobNode_->getLayout().scale.y);
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            knobNode_->getLayout().setScaleType({utils::Layout::ScaleType::PX, utils::Layout::ScaleType::REL});
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            knobNode_->getLayout().setScaleType({utils::Layout::ScaleType::REL, utils::Layout::ScaleType::PX});
        }
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    };
}

Slider& Slider::setColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSlideFrom(const float value)
{
    slideFrom_ = value;
    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSlideTo(const float value)
{
    slideTo_ = value;
    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSlideCurrentValue(const float value)
{
    slideValue_ = value;
    knobOffsetPerc_ = Utils::remap(slideValue_, slideFrom_, slideTo_, 0.0f, 1.0f);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 Slider::getColor() const { return color_; }

glm::vec4 Slider::getBorderColor() const { return borderColor_; }

float Slider::getSlideFrom() const { return slideFrom_; }

float Slider::getSlideTo() const { return slideTo_; }

float Slider::getSlideCurrentValue() const { return slideValue_; }

SliderKnobWPtr Slider::getKnob() {return knobNode_; }

float Slider::getOffsetPerc() const { return knobOffsetPerc_; }
} // msgui