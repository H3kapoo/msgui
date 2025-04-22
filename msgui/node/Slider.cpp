#include "Slider.hpp"

#include "msgui/common/Constants.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/events/Scroll.hpp"
#include "msgui/events/WheelScroll.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/utils/SliderKnob.hpp"

namespace msgui
{
Slider::Slider(const std::string& name) : AbstractNode(name, NodeType::SLIDER)
{
    /* Setup defaults */
    log_ = Logger("Slider(" + name + ")");

    setType(AbstractNode::NodeType::SLIDER);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    color_ = Utils::hexToVec4("#f9f8f7a2");
    layout_.setNewScale({200_px, 20_px});

    textViewPred_ = [](float val) -> std::string
    {
        return std::to_string(int32_t(val));
    };

    knobNode_ = std::make_shared<SliderKnob>("Knob");
    knobNode_->setColor(Utils::hexToVec4("#ee0000ff"));
    knobNode_->getLayout()
        .setNewScale({20_px, 1.0_rel});
    append(knobNode_);

    enableViewValue(true);
    setupLayoutReloadables();

    /* Register only the events you need. */
    getEvents().listen<events::WheelScroll, events::InputChannel>(
        std::bind(&Slider::onMouseWheel, this, std::placeholders::_1));
    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&Slider::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InputChannel>(
        std::bind(&Slider::onMouseDrag, this, std::placeholders::_1));
    getEvents().listen<events::LMBClick, events::InternalChannel>(
        std::bind(&Slider::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InternalChannel>(
        std::bind(&Slider::onMouseRelease, this, std::placeholders::_1));
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
            transform_.pos.y + knobHalf.y, transform_.pos.y + transform_.scale.y - knobHalf.y, common::ZERO, common::ONE);

        /* In scrollBar mode, the knob offset is not inverted since the values start
           increasing from top to bottom as opposed to values in Slider mode. */
        knobOffsetPerc_ = getType() == AbstractNode::NodeType::SLIDER
            ? 1.0f - knobOffsetPerc_
            : knobOffsetPerc_;
    }
    else if (layout_.type == utils::Layout::Type::HORIZONTAL)
    {
        knobOffsetPerc_ = Utils::remap(getState()->mouseX - mouseDistFromKnobCenter_.x,
            transform_.pos.x + knobHalf.x, transform_.pos.x + transform_.scale.x - knobHalf.x, common::ZERO, common::ONE);
    }

    slideValue_ = Utils::remap(knobOffsetPerc_, 0.0f, 1.0f, slideFrom_, slideTo_);
    updateTextValue();

    events::Scroll evt{slideValue_};
    getEvents().notifyAllChannels<events::Scroll>(evt);
}

void Slider::updateTextValue()
{
    if (textLabel_ && textViewPred_) { textLabel_->setText(textViewPred_(slideValue_)); }
}

void Slider::onMouseWheel(const events::WheelScroll& evt)
{
    /* In scrollBar mode, the wheel direction needs to be inverted since the values start
       increasing from top to bottom as opposed to values in Slider mode. */
    getType() == AbstractNode::NodeType::SLIDER
        ? setSlideCurrentValue(slideValue_ + evt.value * sensitivity_)
        : setSlideCurrentValue(slideValue_ - evt.value * sensitivity_);

    events::Scroll scrollEv{slideValue_};
    getEvents().notifyAllChannels<events::Scroll>(scrollEv);
}

void Slider::onMouseClick(const events::LMBClick&)
{
    /* Compute distance offset to the knob center for more natural knob dragging behavior. */
    glm::vec2 knobHalf = glm::vec2{knobNode_->getTransform().scale.x / 2, knobNode_->getTransform().scale.y / 2};
    glm::vec2 kPos = knobNode_->getTransform().pos;
    mouseDistFromKnobCenter_.x = getState()->mouseX - (kPos.x + knobHalf.x);
    mouseDistFromKnobCenter_.x = std::abs(mouseDistFromKnobCenter_.x) > knobHalf.x
        ? 0 : mouseDistFromKnobCenter_.x;
    mouseDistFromKnobCenter_.y = getState()->mouseY - (kPos.y + knobHalf.y);
    mouseDistFromKnobCenter_.y = std::abs(mouseDistFromKnobCenter_.y) > knobHalf.y
        ? 0 : mouseDistFromKnobCenter_.y;

    updateSliderValue();
    MAKE_LAYOUT_DIRTY
}

void Slider::onMouseRelease(const events::LMBRelease& evt)
{
    events::LMBRelease ev{evt};
    getEvents().notifyEvent<events::LMBRelease, events::UserChannel>(ev);
}

void Slider::onMouseDrag(const events::LMBDrag&)
{
    updateSliderValue();
    MAKE_LAYOUT_DIRTY
}

void Slider::setupLayoutReloadables()
{
    auto updateCb = [this](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME; };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
    layout_.onNewScaleChange = [this]
    {
        auto& knobLayout = knobNode_->getLayout();
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            knobLayout.setNewScale({layout_.newScale.y, 1.0_rel});
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            knobLayout.setNewScale({1.0_rel, layout_.newScale.x});
        }
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    };

    layout_.onTypeChange = [this]()
    {
        auto& knobLayout = knobNode_->getLayout();
        if (layout_.type == utils::Layout::Type::VERTICAL &&
            knobNode_->getLayout().newScale.x.type == utils::Layout::ScaleType::REL)
        {
            std::swap(layout_.newScale.x, layout_.newScale.y);
            std::swap(knobLayout.newScale.x, knobLayout.newScale.y);
        }
        else if (layout_.type == utils::Layout::Type::HORIZONTAL &&
            knobNode_->getLayout().newScale.y.type == utils::Layout::ScaleType::REL)
        {
            std::swap(layout_.newScale.x, layout_.newScale.y);
            std::swap(knobLayout.newScale.x, knobLayout.newScale.y);
        }

        setSlideCurrentValue(slideValue_);
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
    slideValue_ = Utils::remap(knobOffsetPerc_, common::ZERO, common::ONE, slideFrom_, slideTo_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSlideTo(const float value)
{
    slideTo_ = value;
    slideValue_ = Utils::remap(knobOffsetPerc_, common::ZERO, common::ONE, slideFrom_, slideTo_);
    updateTextValue();
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSlideCurrentValue(const float value)
{
    slideValue_ = std::clamp(value, slideFrom_, slideTo_);
    knobOffsetPerc_ = Utils::remap(slideValue_, slideFrom_, slideTo_, common::ZERO, common::ONE);

    updateTextValue();

    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::setSensitivity(const float value)
{
    if (value > 0.0f && value <= 100.0f)
    {
        sensitivity_ = value;
    }
    return *this;
}

Slider& Slider::setViewValueFormatPredicate(const std::function<std::string(float)>& pred)
{
    textViewPred_ = pred;
    updateTextValue();

    REQUEST_NEW_FRAME;
    return *this;
}

Slider& Slider::enableViewValue(const bool value)
{
    isViewValueEnabled_ = value;

    if (isViewValueEnabled_)
    {
        textLabel_ = Utils::make<TextLabel>("internalLabel");
        textLabel_->setFontSize(12);
        textLabel_->setEventTransparent(true);
        textLabel_->getLayout()
            .setNewScale({1.0_rel, 1.0_rel});

        updateTextValue();

        knobNode_->append(textLabel_);
        knobNode_->getLayout().setAlignChild(utils::Layout::Align::CENTER);
    }
    else if (!isViewValueEnabled_ && textLabel_)
    {
        knobNode_->remove(textLabel_->getId());
        textLabel_.reset();
    }
    return *this;
}

Slider& Slider::enableDynamicKnob(const bool value)
{
    dynamicKnobEnabled_ = value;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 Slider::getColor() const { return color_; }

glm::vec4 Slider::getBorderColor() const { return borderColor_; }

float Slider::getSlideFrom() const { return slideFrom_; }

float Slider::getSlideTo() const { return slideTo_; }

float Slider::getSlideCurrentValue() const { return slideValue_; }

float Slider::getOffsetPerc() const { return knobOffsetPerc_; }

bool Slider::isDyanmicKnobEnabled() const { return dynamicKnobEnabled_; }

SliderKnobWPtr Slider::getKnob() { return knobNode_; }

TextLabelWPtr Slider::getTextLabel() { return textLabel_; }
} // msgui