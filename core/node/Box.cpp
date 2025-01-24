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
    if (overflow.x > 0 && !hScrollBar_ && layout_.allowOverflow.x)
    {
        hScrollBar_ = std::make_shared<ScrollBar>("HBar", ScrollBar::Orientation::HORIZONTAL);
        hScrollBar_->setScrollbarSize(props.scrollBarSize);
        append(hScrollBar_);
    }
    else if ((overflow.x <= 0 || !layout_.allowOverflow.x) && hScrollBar_)
    {
        remove(hScrollBar_->getId());
        hScrollBar_.reset();
    }
    // Handle vertical OF
    else if (overflow.y > 0 && !vScrollBar_ && layout_.allowOverflow.y)
    {
        vScrollBar_ = std::make_shared<ScrollBar>("VBar", ScrollBar::Orientation::VERTICAL);
        vScrollBar_->setScrollbarSize(props.scrollBarSize);
        append(vScrollBar_);
    }
    else if ((overflow.y <= 0 || !layout_.allowOverflow.y) && vScrollBar_)
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
    shader_->setVec4f("uBorderSize", layout_.border);
    shader_->setVec4f("uBorderRadii", layout_.borderRadius);
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

Box::Props& Box::setColor(const glm::vec4& color)
{
    props.color = color;
    return props;
}

Box::Props& Box::setBorderColor(const glm::vec4& color)
{
    props.borderColor = color;
    return props;
}

Box::Props& Box::setScrollbarSize(const int32_t size)
{
    props.scrollBarSize = size;
    if (hScrollBar_) { hScrollBar_->setScrollbarSize(size); }
    if (vScrollBar_) { vScrollBar_->setScrollbarSize(size); }
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return props;
}

glm::vec4 Box::getColor() const { return props.color; }

glm::vec4 Box::getBorderColor() const { return props.borderColor; }

int32_t Box::getScrollbarSize() const { return props.scrollBarSize; }
} // namespace msgui