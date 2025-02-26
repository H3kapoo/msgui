#include "SliderKnob.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBDrag.hpp"
#include "msgui/nodeEvent/NodeEventManager.hpp"

namespace msgui
{
SliderKnob::SliderKnob(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    /* Register only the events you need. */
    getEvents().listen<nodeevent::LMBClick, nodeevent::InputChannel>(
        std::bind(&SliderKnob::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBDrag, nodeevent::InputChannel>(
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

void SliderKnob::onMouseClick(const nodeevent::LMBClick& evt)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    nodeevent::LMBClick ev{evt};
    sbParentRaw->getEvents().notifyEvent<nodeevent::LMBClick, nodeevent::InternalChannel>(ev);
}

void SliderKnob::onMouseDrag(const nodeevent::LMBDrag& evt)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    nodeevent::LMBDrag ev{evt};
    sbParentRaw->getEvents().notifyEvent<nodeevent::LMBDrag, nodeevent::InternalChannel>(ev);
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