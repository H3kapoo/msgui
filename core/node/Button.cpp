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
    // btnTex_ = TextureLoader::loadTexture("assets/textures/wall.jpg");

    setupReloadables();
}

void Button::setShaderAttributes()
{
    transform_.computeModelMatrix();
    if (btnTex_)
    {
        shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex_->getId());
    }
    shader_->setVec4f("uColor", props.color);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

void* Button::getProps()
{
    return (void*)&props;
}

void Button::onMouseButtonNotify()
{
    listeners.callOnMouseButton(
        state_->lastMouseButtonTriggeredIdx,
        state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
        state_->mouseX,
        state_->mouseY);
}

void Button::setupReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout.alignSelf.onReload = updateCb;
    props.layout.margin.onReload = updateCb;
    props.layout.border.onReload = updateCb;
    props.layout.scaleType.onReload = updateCb;
    props.layout.scale.onReload = updateCb;

    props.texture.onReload = [this]()
    {
        btnTex_ = TextureLoader::loadTexture(props.texture.value);
    };
}
} // msgui