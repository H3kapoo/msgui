#include "SliderKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/Slider.hpp"

namespace msgui
{
SliderKnob::SliderKnob(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::COMMON)
{
    // getLayout().border = Layout::TBLR{4};
    // getLayout().borderRadius = Layout::TBLR{8};
}

void SliderKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", getLayout().border);
    shader_->setVec4f("uBorderRadii", getLayout().borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void SliderKnob::onMouseButtonNotify()
{
    // Pass-through to parent
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    sbParentRaw->onMouseButtonNotify();
}

void SliderKnob::onMouseHoverNotify() {}

void SliderKnob::onMouseDragNotify()
{
    // Pass-through to parent
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
    if (!sbParentRaw) { return; }

    sbParentRaw->onMouseDragNotify();
}

SliderKnob::Props& SliderKnob::setColor(const glm::vec4& color)
{
    props.color = color;
    return props;
}

SliderKnob::Props& SliderKnob::setBorderColor(const glm::vec4& color)
{
    props.borderColor = color;
    return props;
}

glm::vec4 SliderKnob::getColor() const { return props.color; }

glm::vec4 SliderKnob::getBorderColor() const { return props.borderColor; }
} // msgui