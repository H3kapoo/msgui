#include "ScrollBarKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob()
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        "ScrollBarKnob", NodeType::SCROLL_KNOB)
{
    props.layout.border = Layout::TBLR{2, 2, 2, 2};
    props.layout.borderRadius = Layout::TBLR{2, 2, 2, 2};
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
    shader_->setVec4f("uBorderSize", props.layout.border.value);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius.value);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});

    // shader_->setVec4f("uColor", props.color);
    // shader_->setMat4f("uModelMat", transform_.modelMatrix);
}
} // msgui