#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
Button::Button(const std::string& name) : AbstractNode(name, NodeType::BUTTON)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Button(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");
    pressedColor_ = Utils::hexToVec4("#dadada");
    borderColor_ = Utils::hexToVec4("#D2CCC8");
    disabledColor_ = Utils::hexToVec4("#bbbbbbff");
    currentColor_ = color_;

    layout_.setBorder({1});
    layout_.setBorderRadius({4});
    layout_.setScale({70, 34});

    /* Register only the events you need. */
    getEvents().listen<nodeevent::LMBClick, InputChannel>(
        std::bind(&Button::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBRelease, InputChannel>(
        std::bind(&Button::onMouseRelease, this, std::placeholders::_1));
}

void Button::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", isEnabled_ ? currentColor_ : disabledColor_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Button::onMouseClick(const nodeevent::LMBClick&)
{
    currentColor_ = pressedColor_;
    borderColor_ = Utils::hexToVec4("#D2CCC8");

    transform_.pos.x += shrinkFactor;
    transform_.pos.y += shrinkFactor;
    transform_.scale.x -= shrinkFactor*2;
    transform_.scale.y -= shrinkFactor*2;
}

void Button::onMouseRelease(const nodeevent::LMBRelease&)
{
    currentColor_ = color_;
    borderColor_ = Utils::hexToVec4("#D2CCC8");

    transform_.pos.x -= shrinkFactor;
    transform_.pos.y -= shrinkFactor;
    transform_.scale.x += shrinkFactor*2;
    transform_.scale.y += shrinkFactor*2;
}

void Button::setupLayoutReloadables()
{
    auto updateCb = [this](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
}

Button& Button::setColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Button& Button::setPressedColor(const glm::vec4& color)
{
    pressedColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Button& Button::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Button& Button::setTexture(const std::string texturePath)
{
    texturePath_ = texturePath;
    btnTex_ = TextureLoader::loadTexture(texturePath);
    REQUEST_NEW_FRAME;
    return *this;
}

Button& Button::setEnabled(const bool value)
{
    isEnabled_ = value;
    if (isEnabled_)
    {
        currentColor_ = color_;
        getEvents().pauseAll(false);
    }
    else
    {
        currentColor_ = disabledColor_;
        getEvents().pauseAll();
    }

    REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 Button::getColor() const { return color_; }

glm::vec4 Button::getBorderColor() const { return borderColor_; }

std::string Button::getTexturePath() const { return texturePath_; }
} // msgui