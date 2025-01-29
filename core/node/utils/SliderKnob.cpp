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
    shader_->setVec4f("uColor", color_);
    shader_->setVec4f("uBorderColor", borderColor_);
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