#include "ScrollBarKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob() : AbstractNode("ScrollBarKnob", NodeType::SCROLL_KNOB)
{
    setShader(ShaderLoader::load("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    getLayout().border = Layout::TBLR{4};
    getLayout().borderRadius = Layout::TBLR{8};
}

void ScrollBarKnob::setShaderAttributes()
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

void ScrollBarKnob::onMouseButtonNotify()
{
    // Pass-through to parent
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    ScrollBar* sbParentRaw = static_cast<ScrollBar*>(sbParent.get());
    if (!sbParentRaw) { return; }

    sbParentRaw->onMouseButtonNotify();
}

void ScrollBarKnob::onMouseHoverNotify() {}

void ScrollBarKnob::onMouseDragNotify()
{
    // Pass-through to parent
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    ScrollBar* sbParentRaw = static_cast<ScrollBar*>(sbParent.get());
    if (!sbParentRaw) { return; }

    sbParentRaw->onMouseDragNotify();
}

ScrollBarKnob& ScrollBarKnob::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

ScrollBarKnob& ScrollBarKnob::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

glm::vec4 ScrollBarKnob::getColor() const { return color_; }

glm::vec4 ScrollBarKnob::getBorderColor() const { return borderColor_; }
} // msgui