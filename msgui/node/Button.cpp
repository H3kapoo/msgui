#include "Button.hpp"

#include <GLFW/glfw3.h>

#include "msgui/events/FocusLost.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include "msgui/events/MouseEnter.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/TextLabel.hpp"

namespace msgui
{
Button::Button(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    /* Setup defaults */
    log_ = ("Button(" + name + ")");

    setType(AbstractNode::NodeType::COMMON);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    color_ = Utils::hexToVec4("#F9F8F7FF");
    pressedColor_ = Utils::hexToVec4("#dadadaff");
    borderColor_ = Utils::hexToVec4("#D2CCC8ff");
    disabledColor_ = Utils::hexToVec4("#bbbbbbff");
    currentColor_ = color_;

    layout_.setNewScale({70_px, 34_px});

    /* Register only the events you need. */
    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&Button::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&Button::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<events::LMBReleaseNotHovered, events::InputChannel>(
        std::bind(&Button::onMouseReleaseNotHovered, this, std::placeholders::_1));
    getEvents().listen<events::MouseEnter, events::InputChannel>(
        std::bind(&Button::onMouseEnter, this, std::placeholders::_1));
    getEvents().listen<events::MouseExit, events::InputChannel>(
        std::bind(&Button::onMouseExit, this, std::placeholders::_1));
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

void Button::onMouseClick(const events::LMBClick&)
{
    currentColor_ = pressedColor_;

    layout_.shrink = {2, 2};
    MAKE_LAYOUT_DIRTY;
}

void Button::onMouseRelease(const events::LMBRelease&)
{
    currentColor_ = color_;

    layout_.shrink = {0, 0};
    MAKE_LAYOUT_DIRTY;
}

void Button::onMouseReleaseNotHovered(const events::LMBReleaseNotHovered&)
{
    /* In the particular case of receiving the event from Input, LMBReleaseNotHovered acts just like LMBRelease. */
    const events::LMBRelease evt{{0,0}};
    onMouseRelease(evt);
}

void Button::onMouseEnter(const events::MouseEnter&)
{
    currentColor_ = Utils::darken(color_, 0.2f);
}

void Button::onMouseExit(const events::MouseExit&)
{
    currentColor_ = Utils::lighten(color_, 0.2f);
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
        getEvents().pauseEvent<events::FocusLost, events::InternalChannel>(false);
    }

    REQUEST_NEW_FRAME;
    return *this;
}

Button& Button::setText(const std::string& text)
{
    /* Remove the label if text is empty. */
    if (!text.size())
    {
        if (textLabel_)
        {
            remove(textLabel_->getId());
            textLabel_.reset();
        }
        return *this;
    }

    if (!textLabel_)
    {
        textLabel_ = Utils::make<TextLabel>(getName() + "_Label");
        textLabel_->setEventTransparent(true);
        textLabel_->getLayout().setNewScale({1_fill});
        append(textLabel_);
    }

    textLabel_->setText(text);

    return *this;
}

Button& Button::setImagePath(const std::string& path)
{
    /* Remove the image if path is empty. */
    if (!path.size())
    {
        if (image_)
        {
            remove(image_->getId());
            image_.reset();
        }
        return *this;
    }

    if (!image_)
    {
        image_ = Utils::make<Image>(getName() + "_Image");
        image_->setImage(path);
        image_->setEventTransparent(true);
        image_->getLayout().setNewScale({1_fill});

        append(image_);
    }

    return *this;
}

glm::vec4 Button::getColor() const { return color_; }

glm::vec4 Button::getBorderColor() const { return borderColor_; }

std::string Button::getText() const { return textLabel_ ? textLabel_->getText() : ""; }

std::string Button::getImagePath() const { return image_->getImagePath(); }

TextLabelWPtr Button::getTextLabel() { return textLabel_; }

ImageWPtr Button::getImage() {return image_; }
} // msgui