#include "Box.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
Box::Box(const std::string& name) : AbstractNode(name, NodeType::BOX)
{
    log_ = Logger("Box(" + name +")");
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
}

bool Box::isScrollBarActive(const ScrollBar::Type orientation)
{
    switch (orientation)
    {
        case ScrollBar::Type::HORIZONTAL:
            return hScrollBar_ ? true : false;
        case ScrollBar::Type::VERTICAL:
            return vScrollBar_ ? true : false;
        case ScrollBar::Type::ALL:
            return (hScrollBar_ && vScrollBar_);
        case ScrollBar::Type::NONE:
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
        hScrollBar_ = std::make_shared<ScrollBar>("HBar", ScrollBar::Type::HORIZONTAL);
        hScrollBar_->setScrollbarSize(scrollBarSize_);
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
        vScrollBar_ = std::make_shared<ScrollBar>("VBar", ScrollBar::Type::VERTICAL);
        vScrollBar_->setScrollbarSize(scrollBarSize_);
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
        getState()->isLayoutDirty = hScrollBar_->setOverflow(overflow.x);
    }

    if (vScrollBar_)
    {
        getState()->isLayoutDirty = vScrollBar_->setOverflow(overflow.y);
    }
}

void Box::setShaderAttributes()
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

void Box::onMouseButtonNotify()
{
    listeners_.callOnMouseButton(
        getState()->lastMouseButtonTriggeredIdx,
        getState()->mouseButtonState[getState()->lastMouseButtonTriggeredIdx],
        getState()->mouseX,
        getState()->mouseY);
    // log_.infoLn("I was clicked at %d %d", getState()->mouseX, getState()->mouseY);
}

void Box::onMouseDragNotify()
{}

Box& Box::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

Box& Box::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

Box& Box::setScrollbarSize(const int32_t size)
{
    scrollBarSize_ = size;
    if (hScrollBar_) { hScrollBar_->setScrollbarSize(size); }
    if (vScrollBar_) { vScrollBar_->setScrollbarSize(size); }
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

glm::vec4 Box::getColor() const { return color_; }

glm::vec4 Box::getBorderColor() const { return borderColor_; }

int32_t Box::getScrollbarSize() const { return scrollBarSize_; }

ScrollBarPtr Box::getScrollBar(const ScrollBar::Type orientation)
{
    if (orientation == ScrollBar::Type::HORIZONTAL) { return hScrollBar_; }
    if (orientation == ScrollBar::Type::VERTICAL) { return vScrollBar_; }

    return nullptr;
}

Listeners& Box::getListeners() { return listeners_; }

} // namespace msgui