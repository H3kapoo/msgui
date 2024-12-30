#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"

namespace msgui
{
Button::Button(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basicTex.glsl"), name)
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

// ---- Listeners ---- //
void Button::setMouseClickListener(std::function<void()> cb)
{
    mouseClickCb_ = cb;
}

// ---- Overrides Private ---- //
void Button::onMouseButtonNotify()
{
    if (not state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT] && mouseClickCb_)
    {
        mouseClickCb_();
    }
}
} // msgui