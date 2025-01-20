#include "Box.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/ScrollBar.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{
Box::Box(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"), name, NodeType::BOX)
{
    log_ = Logger("Box(" + name +")");
    transform_.scale = {100, 100, 1};
    transform_.pos = {100, 100, 2};

    setupReloadables();
}

void* Box::getProps()
{
    return &props;
}

bool Box::isScrollBarActive(const ScrollBar::Orientation orientation)
{
    switch (orientation)
    {
        case ScrollBar::Orientation::HORIZONTAL:
            return hScrollBar_ ? true : false;
        case ScrollBar::Orientation::VERTICAL:
            return vScrollBar_ ? true : false;
        case ScrollBar::Orientation::ALL:
            return (hScrollBar_ && vScrollBar_);
        case ScrollBar::Orientation::NONE:
            return (!hScrollBar_ && !vScrollBar_);
        default:
            log_.errorLn("Invalid orientation!");
    }

    return false;
}

void Box::updateOverflow(const glm::ivec2& overflow)
{
    overflow_ = overflow;

    // Handle horizontal OF
    if (overflow.x > 0 && !hScrollBar_ && props.layout.allowOverflow.x)
    {
        hScrollBar_ = std::make_shared<ScrollBar>("HBar", ScrollBar::Orientation::HORIZONTAL);
        hScrollBar_->props.sbSize = props.scrollBarSize.value;
        append(hScrollBar_);
    }
    else if ((overflow.x <= 0 || !props.layout.allowOverflow.x) && hScrollBar_)
    {
        remove(hScrollBar_->getId());
        hScrollBar_.reset();
    }
    // Handle vertical OF
    else if (overflow.y > 0 && !vScrollBar_ && props.layout.allowOverflow.y)
    {
        vScrollBar_ = std::make_shared<ScrollBar>("VBar", ScrollBar::Orientation::VERTICAL);
        vScrollBar_->props.sbSize = props.scrollBarSize.value;
        append(vScrollBar_);
    }
    else if ((overflow.y <= 0 || !props.layout.allowOverflow.y) && vScrollBar_)
    {
        remove(vScrollBar_->getId());
        vScrollBar_.reset();
    }

    // Update with the new overflow value
    if (hScrollBar_)
    {
        state_->isLayoutDirty = hScrollBar_->setOverflow(overflow.x);
    }

    if (vScrollBar_)
    {
        state_->isLayoutDirty = vScrollBar_->setOverflow(overflow.y);
    }
}

void Box::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Box::onMouseButtonNotify()
{
    listeners.callOnMouseButton(
        state_->lastMouseButtonTriggeredIdx,
        state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
        state_->mouseX,
        state_->mouseY);
    // log_.infoLn("I was clicked at %d %d", state_->mouseX, state_->mouseY);
}

void Box::onMouseDragNotify()
{}

void Box::setupReloadables()
{
    props.layout._onAllowOverflowChange = [this]()
    {
        props.layout.allowOverflow.x ? updateOverflow(overflow_) : updateOverflow({0, 0});
        props.layout.allowOverflow.y ? updateOverflow(overflow_) : updateOverflow({0, 0});
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout._onTypeChange = updateCb;
    props.layout._onAllowWrapChange = updateCb;
    props.layout._onAlignSelfChange = updateCb;
    props.layout._onMarginChange = updateCb;
    props.layout._onBorderChange = updateCb;
    props.layout._onScaleTypeChange = updateCb;
    props.layout._onScaleChange = updateCb;

    props.scrollBarSize.onReload = [this, updateCb]()
    {
        if (hScrollBar_) { hScrollBar_->props.sbSize = props.scrollBarSize.value; }
        if (vScrollBar_) { vScrollBar_->props.sbSize = props.scrollBarSize.value; }
        updateCb();
    };
}
} // namespace msgui