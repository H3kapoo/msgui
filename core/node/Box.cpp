#include "Box.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/ScrollBar.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{
Box::Box(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"), name, NodeType::BOX)
{
    log_ = Logger("Box(" + name +")");
    transform_.setScale({100, 100, 1});
    transform_.setPos({100, 100, 2});

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
            return (hScrollBar_ && vScrollBar_) ? true : false;
        default:
            log_.errorLn("Invalid orientation!");
    }

    return false;
}

void Box::updateOverflow(const glm::ivec2& overflow)
{
    overflow_ = overflow;

    // Update with the new overflow value
    if (hScrollBar_)
    {
        state_->isLayoutDirty = hScrollBar_->setOverflow(overflow.x);
    }

    if (vScrollBar_)
    {
        state_->isLayoutDirty = vScrollBar_->setOverflow(overflow.y);
    }

    // Handle horizontal OF
    if (overflow.x > 0 && !hScrollBar_ && props.layout.allowOverflowX)
    {
        hScrollBar_ = std::make_shared<ScrollBar>("HBar", ScrollBar::Orientation::HORIZONTAL);
        append(hScrollBar_);
    }
    else if ((overflow.x <= 0 || !props.layout.allowOverflowX) && hScrollBar_)
    {
        remove(hScrollBar_->getId());
        hScrollBar_.reset();
    }
    // Handle vertical OF
    else if (overflow.y > 0 && !vScrollBar_ && props.layout.allowOverflowY)
    {
        vScrollBar_ = std::make_shared<ScrollBar>("VBar", ScrollBar::Orientation::VERTICAL);
        append(vScrollBar_);
    }
    else if ((overflow.y <= 0 || !props.layout.allowOverflowY) && vScrollBar_)
    {
        remove(vScrollBar_->getId());
        vScrollBar_.reset();
    }
}

void Box::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", props.color);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
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

void Box::setupReloadables()
{
    props.layout.allowOverflowX.onReload = [this]()
    {
        props.layout.allowOverflowX ? updateOverflow(overflow_) : updateOverflow({0, 0});
    };

    props.layout.allowOverflowY.onReload = [this]()
    {
        props.layout.allowOverflowY ? updateOverflow(overflow_) : updateOverflow({0, 0});
    };

    props.layout.orientation.onReload = [this]()
    {
        state_->isLayoutDirty = true;
    };
}
} // namespace msgui