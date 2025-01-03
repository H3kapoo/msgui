#include "ScrollBarKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob()
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"),
        "ScrollBarKnob", NodeType::SCROLL_KNOB)
{}

void* ScrollBarKnob::getProps()
{
    return &props;
}

void ScrollBarKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", props.color);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}
} // msgui