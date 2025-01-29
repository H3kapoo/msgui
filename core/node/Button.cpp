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

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#bbbbbbff");
    borderColor_ = Utils::hexToVec4("#55bbbbff");
    // layout_.border = Layout::TBLR{5, 2, 5, 2};
    // layout_.border = Layout::TBLR{2, 5, 2, 5};
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
    shader_->setVec4f("uColor", color_);
    shader_->setVec4f("uBorderColor", borderColor_);
    shader_->setVec4f("uBorderSize", layout_.border);
    shader_->setVec4f("uBorderRadii", layout_.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Button::onMouseButtonNotify()
{
    // Predefined behavior
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // layout_.border = Layout::TBLR{5, 2, 5, 2};
        // props.borderColor = Utils::hexToVec4("#dd0000ff");
    }
    else if (!state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // layout_.border = Layout::TBLR{2, 5, 2, 5};
        // props.borderColor = Utils::hexToVec4("#55bbbbff");
    }

    // User custom behavior
    listeners_.callOnMouseButton(
        state_->lastMouseButtonTriggeredIdx,
        state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
        state_->mouseX,
        state_->mouseY);
}

void Button::setupLayoutReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

Button& Button::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

Button& Button::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

Button& Button::setTexture(const std::string texturePath)
{
    texturePath_ = texturePath;
    btnTex_ = TextureLoader::loadTexture(texturePath);
    return *this;
}

glm::vec4 Button::getColor() const { return color_; }

glm::vec4 Button::getBorderColor() const { return borderColor_; }

std::string Button::getTexturePath() const { return texturePath_; }

Listeners& Button::getListeners() { return listeners_; }
} // msgui