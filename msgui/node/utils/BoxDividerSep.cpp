#include "BoxDividerSep.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/utils/LayoutData.hpp"

namespace msgui
{
BoxDividerSep::BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox)
    : AbstractNode(name, NodeType::BOX_DIVIDER_SEP)
    , firstBox_(firstBox)
    , secondBox_(secondBox)
{
    log_ = ("BoxDividerSep(" + name + ")");
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    //TODO: Box divider should not be "active" with < 2 boxes
    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#52161bff");
    layout_.onTypeChange();

    getEvents().listen<nodeevent::LMBClick, nodeevent::InputChannel>(
        std::bind(&BoxDividerSep::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBRelease, nodeevent::InputChannel>(
        std::bind(&BoxDividerSep::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBDrag, nodeevent::InputChannel>(
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

void BoxDividerSep::onMouseClick(const nodeevent::LMBClick&)
{
    if (layout_.type == Layout::Type::HORIZONTAL)
    {
        getState()->currentCursorId = GLFW_HRESIZE_CURSOR;
    }
    else if (layout_.type == Layout::Type::VERTICAL)
    {
        getState()->currentCursorId = GLFW_VRESIZE_CURSOR;
    }
}

void BoxDividerSep::onMouseRelease(const nodeevent::LMBRelease&)
{
    getState()->currentCursorId = GLFW_ARROW_CURSOR;
}

void BoxDividerSep::onMouseDrag(const nodeevent::LMBDrag&)
{
    Layout& left = firstBox_->getLayout();
    Layout& right = secondBox_->getLayout();
    // Temp is used here as we don't want to modify the original scale supplied by the user
    // Maybe there's a better way to do it..later.

    if (layout_.type == Layout::Type::HORIZONTAL)
    {
        float diff = getState()->mouseX - getState()->lastMouseX;
        left.tempScale.x += diff;
        right.tempScale.x -= diff;
    }
    else if (layout_.type == Layout::Type::VERTICAL)
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

//     if (layout_.type == Layout::Type::HORIZONTAL)
//     {
//         float diff = getState()->mouseX - getState()->lastMouseX;
//         left.tempScale.x += diff;
//         right.tempScale.x -= diff;
//     }
//     else if (layout_.type == Layout::Type::VERTICAL)
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
        if (layout_.type == Layout::Type::HORIZONTAL)
        {
            layout_.scaleType = {Layout::ScaleType::ABS, Layout::ScaleType::REL};
            layout_.scale = {10, 1.0f};
        }
        else if (layout_.type == Layout::Type::VERTICAL)
        {
            layout_.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::ABS};
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