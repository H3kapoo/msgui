#include "BoxDividerSep.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"

namespace msgui
{
BoxDividerSep::BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox)
    : AbstractNode(name, NodeType::BOX_DIVIDER_SEP)
    , firstBox_(firstBox)
    , secondBox_(secondBox)
{
    log_ = ("BoxDividerSep(" + name + ")");
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    //TODO: Box divider should not be "active" with < 2 boxes
    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#52161bff");
    layout_.onTypeChange();

    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&BoxDividerSep::onMouseRelease, this, std::placeholders::_1));
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

// void BoxDividerSep::onMouseDragNotify()
// {
//     Layout& left = firstBox_->getLayout();
//     Layout& right = secondBox_->getLayout();
//     // Temp is used here as we don't want to modify the original scale supplied by the user
//     // Maybe there's a better way to do it..later.

//     if (layout_.type == utils::Layout::Type::HORIZONTAL)
//     {
//         float diff = getState()->mouseX - getState()->lastMouseX;
//         left.tempScale.x += diff;
//         right.tempScale.x -= diff;
//     }
//     else if (layout_.type == utils::Layout::Type::VERTICAL)
//     {
//         float diff = getState()->mouseY - getState()->lastMouseY;
//         left.tempScale.y += diff;
//         right.tempScale.y -= diff;
//     }

//     isActiveSeparator_ = true;
//     MAKE_LAYOUT_DIRTY
// }

void BoxDividerSep::setupLayoutReloadables()
{
    layout_.onTypeChange = [this]()
    {
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            layout_.scaleType = {utils::Layout::ScaleType::PX, utils::Layout::ScaleType::REL};
            layout_.scale = {10, 1.0f};
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            layout_.scaleType = {utils::Layout::ScaleType::REL, utils::Layout::ScaleType::PX};
            layout_.scale = {1.0f, 10};
        }
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
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

bool BoxDividerSep::getIsActiveSeparator()
{
    bool val = isActiveSeparator_;
    isActiveSeparator_ = false;
    return val;
}

BoxPtr BoxDividerSep::getFirstBox() { return firstBox_; }

BoxPtr BoxDividerSep::getSecondBox() { return secondBox_; }

} // msgui