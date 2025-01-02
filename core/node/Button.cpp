#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"

namespace msgui
{
Button::Button(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basicTex.glsl"), name, NodeType::BUTTON)
{
    log_ = ("Button(" + name + ")");
    btnTex = TextureLoader::loadTexture("assets/textures/wall.jpg");
}

// ---- Overrides ---- //
void Button::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex->getId());
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

// ---- Virtual Getters ---- //
void* Button::getProps()
{
    // No props yet
    return nullptr;
}

// ---- Getters ---- //
Listeners& Button::getListeners()
{
    return listeners_;
}

// ---- Override Notifiers ---- //
void Button::onMouseButtonNotify()
{
    listeners_.callOnMouseButton(
        state_->lastMouseButtonTriggeredIdx,
        state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
        state_->mouseX,
        state_->mouseY);
}
} // msgui