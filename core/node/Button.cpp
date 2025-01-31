#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"
#include "core/Utils.hpp"

namespace msgui
{
Button::Button(const std::string& name) : AbstractNode(name, NodeType::BUTTON)
{
    setShader(ShaderLoader::load("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
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
    auto shader = getShader();
    // if (btnTex_)
    // {
    //     shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex_->getId());
    // }
    // else
    // {
    //     shader_->setTexture2D("uTexture", GL_TEXTURE0, 0);
    // }
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Button::onMouseButtonNotify()
{
    // Predefined behavior
    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // layout_.border = Layout::TBLR{5, 2, 5, 2};
        // props.borderColor = Utils::hexToVec4("#dd0000ff");
    }
    else if (!getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        // layout_.border = Layout::TBLR{2, 5, 2, 5};
        // props.borderColor = Utils::hexToVec4("#55bbbbff");
    }

    // User custom behavior
    listeners_.callOnMouseButton(
        getState()->lastMouseButtonTriggeredIdx,
        getState()->mouseButtonState[getState()->lastMouseButtonTriggeredIdx],
        getState()->mouseX,
        getState()->mouseY);
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