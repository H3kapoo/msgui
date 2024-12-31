#include "ScrollBarKnob.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob()
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"),
        "KnobTestBar1", NodeType::SCROLL_KNOB)
{}

// ---- Overrides ---- //
void* ScrollBarKnob::getProps()
{
    return nullptr;
}

// ---- Overrides Private ---- //
void ScrollBarKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", color_);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}
} // msgui