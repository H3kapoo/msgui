#include "ScrollBarKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob()
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        "ScrollBarKnob", NodeType::SCROLL_KNOB)
{
    getLayout().border = Layout::TBLR{4};
    getLayout().borderRadius = Layout::TBLR{8};
}

void ScrollBarKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", getLayout().border);
    shader_->setVec4f("uBorderRadii", getLayout().borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
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

ScrollBarKnob::Props& ScrollBarKnob::setColor(const glm::vec4& color)
{
    props.color = color;
    return props;
}

ScrollBarKnob::Props& ScrollBarKnob::setBorderColor(const glm::vec4& color)
{
    props.borderColor = color;
    return props;
}

glm::vec4 ScrollBarKnob::getColor() const { return props.color; }

glm::vec4 ScrollBarKnob::getBorderColor() const { return props.borderColor; }
} // msgui