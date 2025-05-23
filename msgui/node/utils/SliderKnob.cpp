#include "SliderKnob.hpp"

#include "msgui/events/LMBRelease.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/NodeEventManager.hpp"

namespace msgui
{
SliderKnob::SliderKnob(const std::string& name)
    : AbstractNode("SliderKnob(" + name +")", NodeType::SLIDER_KNOB)
{
    log_ = Logger(getName());
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    /* Register only the events you need. */
    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&SliderKnob::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&SliderKnob::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InputChannel>(
        std::bind(&SliderKnob::onMouseDrag, this, std::placeholders::_1));
}

void SliderKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", getLayout().border);
    shader->setVec4f("uBorderRadii", getLayout().borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void SliderKnob::onMouseClick(const events::LMBClick& evt)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    events::LMBClick ev{evt};
    sbParentRaw->getEvents().notifyEvent<events::LMBClick, events::InternalChannel>(ev);
}

void SliderKnob::onMouseRelease(const events::LMBRelease& evt)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    events::LMBRelease ev{evt};
    sbParentRaw->getEvents().notifyEvent<events::LMBRelease, events::InternalChannel>(ev);
}

void SliderKnob::onMouseDrag(const events::LMBDrag& evt)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    events::LMBDrag ev{evt};
    sbParentRaw->getEvents().notifyEvent<events::LMBDrag, events::InternalChannel>(ev);
}

SliderKnob& SliderKnob::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

SliderKnob& SliderKnob::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

glm::vec4 SliderKnob::getColor() const { return color_; }

glm::vec4 SliderKnob::getBorderColor() const { return borderColor_; }
} // msgui