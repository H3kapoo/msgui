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
    props.layout.border = Layout::TBLR{4};
    props.layout.borderRadius = Layout::TBLR{8};
}

void* ScrollBarKnob::getProps()
{
    return &props;
}

void ScrollBarKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
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
} // msgui