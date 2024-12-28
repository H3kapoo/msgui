#include "Button.hpp"

#include "../MeshLoader.hpp"
#include "../ShaderLoader.hpp"
#include "../TextureLoader.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{
Button::Button(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basicTex.glsl"), name)
    // : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"), name)
{
    log_ = ("Button(" + name + ")");
    btnTex = TextureLoader::loadTexture("assets/textures/wall.jpg");
}

void Button::setShaderAttributes()
{
    // AbstractNode::setShaderAttributes();

    transform_.computeModelMatrix();
    shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex->getId());
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

void Button::setMouseClickListener(std::function<void()> cb)
{
    mouseClickCb_ = cb;
}

void Button::onMouseButtonNotify()
{
    if (not state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT] && mouseClickCb_)
    {
        mouseClickCb_();
    }
}



} // msgui