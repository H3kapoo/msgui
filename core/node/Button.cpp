#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"

namespace msgui
{
Button::Button(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"), name, NodeType::BUTTON)
{
    log_ = ("Button(" + name + ")");

    props.color = Utils::hexToVec4("#bbbbbbff");
    props.borderColor = Utils::hexToVec4("#55bbbbff");
    // props.layout.border = Layout::TBLR{5, 2, 5, 2};
    props.layout.border = Layout::TBLR{2, 5, 2, 5};
    setupReloadables();
}

void Button::setShaderAttributes()
{
    transform_.computeModelMatrix();
    // if (btnTex_)
    // {
    //     shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex_->getId());
    // }
    // else
    // {
    //     shader_->setTexture2D("uTexture", GL_TEXTURE0, 0);
    // }
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border.value);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius.value);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* Button::getProps()
{
    return (void*)&props;
}

void Button::onMouseButtonNotify()
{
    // Predefined behavior
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        props.layout.border = Layout::TBLR{5, 2, 5, 2};
        props.borderColor = Utils::hexToVec4("#dd0000ff");
    }
    else if (!state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        props.layout.border = Layout::TBLR{2, 5, 2, 5};
        props.borderColor = Utils::hexToVec4("#660000ff");
    }

    // User custom behavior
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