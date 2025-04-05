#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/TextureLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/LMBReleaseNotHovered.hpp"
#include "msgui/nodeEvent/NodeEventManager.hpp"

namespace msgui
{
Button::Button(const std::string& name) : AbstractNode(name, NodeType::COMMON)
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

    layout_.setScale({70, 34});

    /* Register only the events you need. */
    getEvents().listen<nodeevent::LMBClick, nodeevent::InputChannel>(
        std::bind(&Button::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBRelease, nodeevent::InputChannel>(
        std::bind(&Button::onMouseRelease, this, std::placeholders::_1));
        getEvents().listen<nodeevent::LMBReleaseNotHovered, nodeevent::InputChannel>(
            std::bind(&Button::onMouseReleaseNotHovered, this, std::placeholders::_1));
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

    layout_.shrink = {2, 2};
    MAKE_LAYOUT_DIRTY;
}

void Button::onMouseRelease(const nodeevent::LMBRelease&)
{
    currentColor_ = color_;
    
    layout_.shrink = {0, 0};
    MAKE_LAYOUT_DIRTY;
}

void Button::onMouseReleaseNotHovered(const nodeevent::LMBReleaseNotHovered&)
{
    /* In the particular case of receiving the event from Input, LMBReleaseNotHovered acts just like LMBRelease. */
    const nodeevent::LMBRelease evt{{0,0}};
    onMouseRelease(evt);
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
    currentColor_ = color;
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
        getEvents().pauseAllEvents(false);
    }
    else
    {
        currentColor_ = disabledColor_;
        getEvents().pauseAllEvents();
        /* This is mainly needed in case the Button is a menu item in a dropdown. */
        getEvents().pauseEvent<nodeevent::FocusLost, InternalChannel>(false);
    }

    REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 Button::getColor() const { return color_; }

glm::vec4 Button::getBorderColor() const { return borderColor_; }

std::string Button::getTexturePath() const { return texturePath_; }
} // msgui