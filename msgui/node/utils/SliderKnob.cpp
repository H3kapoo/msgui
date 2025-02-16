#include "SliderKnob.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/node/Slider.hpp"

namespace msgui
{
SliderKnob::SliderKnob(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
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

// void SliderKnob::onMouseButtonNotify()
// {
//     // Pass-through to parent
//     AbstractNodePtr sbParent = parent_.lock();
//     if (!sbParent) { return; }

//     Slider* sbParentRaw = static_cast<Slider*>(sbParent.get());
//     if (!sbParentRaw) { return; }

//     sbParentRaw->onMouseButtonNotify();
// }

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