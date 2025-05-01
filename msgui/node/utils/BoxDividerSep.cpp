#include "BoxDividerSep.hpp"

#include "msgui/events/MouseEnter.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{
BoxDividerSep::BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox)
    : AbstractNode("BoxDividerSep(" + name + ")", NodeType::BOX_DIVIDER_SEP)
    , firstBox_(firstBox)
    , secondBox_(secondBox)
{
    log_ = Logger(getName());
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#52161bff");
    layout_.onTypeChange();

    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<events::MouseEnter, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseEnter, this, std::placeholders::_1));
    getEvents().listen<events::MouseExit, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseExit, this, std::placeholders::_1));
    getEvents().listen<events::LMBReleaseNotHovered, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseReleaseNotHovered, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseDrag, this, std::placeholders::_1));

}

void BoxDividerSep::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void BoxDividerSep::onMouseClick(const events::LMBClick&)
{
    if (layout_.type == utils::Layout::Type::HORIZONTAL)
    {
        getState()->currentCursorId = GLFW_HRESIZE_CURSOR;
    }
    else if (layout_.type == utils::Layout::Type::VERTICAL)
    {
        getState()->currentCursorId = GLFW_VRESIZE_CURSOR;
    }
}

void BoxDividerSep::onMouseRelease(const events::LMBRelease&)
{
    getState()->currentCursorId = GLFW_ARROW_CURSOR;
}

void BoxDividerSep::onMouseEnter(const events::MouseEnter& evt)
{
    if (layout_.type == utils::Layout::Type::HORIZONTAL)
    {
        getState()->currentCursorId = GLFW_HRESIZE_CURSOR;
    }
    else if (layout_.type == utils::Layout::Type::VERTICAL)
    {
        getState()->currentCursorId = GLFW_VRESIZE_CURSOR;
    }
}

void BoxDividerSep::onMouseExit(const events::MouseExit& evt)
{
    if (getState()->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT]) { return; }
    getState()->currentCursorId = GLFW_ARROW_CURSOR;
}

void BoxDividerSep::onMouseReleaseNotHovered(const events::LMBReleaseNotHovered&)
{
    events::LMBRelease evt{{0, 0}};
    onMouseRelease(evt);
}

void BoxDividerSep::onMouseDrag(const events::LMBDrag&)
{
    utils::Layout& left = firstBox_->getLayout();
    utils::Layout& right = secondBox_->getLayout();
    // Temp is used here as we don't want to modify the original scale supplied by the user
    // Maybe there's a better way to do it..later.

    if (layout_.type == utils::Layout::Type::HORIZONTAL)
    {
        float diff = getState()->mouseX - getState()->lastMouseX;
        left.tempScale.x += diff;
        right.tempScale.x -= diff;
    }
    else if (layout_.type == utils::Layout::Type::VERTICAL)
    {
        float diff = getState()->mouseY - getState()->lastMouseY;
        left.tempScale.y += diff;
        right.tempScale.y -= diff;
    }

    isActiveSeparator_ = true;
    MAKE_LAYOUT_DIRTY
}

void BoxDividerSep::setupLayoutReloadables()
{
    layout_.onTypeChange = [this]()
    {
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            layout_.setNewScale({10_px, 1.0_rel});
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            layout_.setNewScale({1.0_rel, 10_px});
        }
    };
}

BoxDividerSep& BoxDividerSep::setColor(const glm::vec4 color)
{
    color_ = color;
    return *this;
}

BoxDividerSep& BoxDividerSep::setBorderColor(const glm::vec4 color)
{
    borderColor_ = color;
    return *this;
}

bool BoxDividerSep::checkIfActiveThenReset()
{
    bool val = isActiveSeparator_;
    isActiveSeparator_ = false;
    return val;
}

BoxPtr BoxDividerSep::getFirstBox() { return firstBox_; }

BoxPtr BoxDividerSep::getSecondBox() { return secondBox_; }

} // msgui