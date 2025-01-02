#include "Box.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"

namespace msgui
{
Box::Box(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"), name, NodeType::BOX)
{
    log_ = Logger("Box(" + name +")");
    transform_.setScale({100, 100, 1});
    transform_.setPos({100, 100, 2});
}

// ---- Overrides ---- //
void* Box::getProps()
{
    return (void*)&props_;
}

// ---- Setters ---- //
void Box::setColor(const glm::vec4& color)
{
    color_ = color;
}

// ---- Getters ---- //
Listeners& Box::getListeners()
{
    return listeners_;
}

// ---- Overrides Private ---- //
void Box::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", color_);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

void Box::onMouseButtonNotify()
{
    listeners_.callOnMouseButton(
        state_->lastMouseButtonTriggeredIdx,
        state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
        state_->mouseX,
        state_->mouseY);
    // log_.infoLn("I was clicked at %d %d", state_->mouseX, state_->mouseY);
}
} // namespace msgui