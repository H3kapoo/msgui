#include "BoxDividerSep.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/LayoutData.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{
BoxDividerSep::BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::BOX_DIVIDER_SEP)
    , firstBox_(firstBox)
    , secondBox_(secondBox)
{
    log_ = ("BoxDividerSep(" + name + ")");

    //TODO: Box divider should not be "active" with < 2 boxes
    setupReloadables();

    props.color = Utils::hexToVec4("#52161bff");
    props.layout._onTypeChange();
}

void BoxDividerSep::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* BoxDividerSep::getProps() { return (void*)&props; }

BoxPtr BoxDividerSep::getFirstBox() { return firstBox_; }

BoxPtr BoxDividerSep::getSecondBox() { return secondBox_; }

void BoxDividerSep::onMouseButtonNotify()
{
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        if (props.layout.type == Layout::Type::HORIZONTAL)
        {
            state_->currentCursorId = GLFW_HRESIZE_CURSOR;
        }
        else if (props.layout.type == Layout::Type::VERTICAL)
        {
            state_->currentCursorId = GLFW_VRESIZE_CURSOR;
        }
    }
    else
    {
        state_->currentCursorId = GLFW_ARROW_CURSOR;
    }
}

void BoxDividerSep::onMouseDragNotify()
{
    Layout* left = static_cast<Layout*>(firstBox_->getProps());
    Layout* right = static_cast<Layout*>(secondBox_->getProps());
    // Temp is used here as we don't want to modify the original scale supplied by the user
    // Maybe there's a better way to do it..later.

    if (props.layout.type == Layout::Type::HORIZONTAL)
    {
        float diff = state_->mouseX - state_->lastMouseX;
        left->tempScale.x += diff;
        right->tempScale.x -= diff;
    }
    else if (props.layout.type == Layout::Type::VERTICAL)
    {
        float diff = state_->mouseY - state_->lastMouseY;
        left->tempScale.y += diff;
        right->tempScale.y -= diff;
    }

    props.isActiveSeparator = true;
    MAKE_LAYOUT_DIRTY
}

void BoxDividerSep::setupReloadables()
{
    props.layout._onTypeChange = [this]()
    {
        if (props.layout.type == Layout::Type::HORIZONTAL)
        {
            props.layout.scaleType = {Layout::ScaleType::ABS, Layout::ScaleType::REL};
            props.layout.scale = {10, 1.0f};
        }
        else if (props.layout.type == Layout::Type::VERTICAL)
        {
            props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::ABS};
            props.layout.scale = {1.0f, 10};
        }
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout._onAlignSelfChange = updateCb;
    props.layout._onMarginChange = updateCb;
    props.layout._onBorderChange = updateCb;
    props.layout._onScaleTypeChange = updateCb;
    props.layout._onScaleChange = updateCb;
}
} // msgui