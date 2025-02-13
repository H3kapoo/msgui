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

    setupReloadables();
}

bool Box::isScrollBarActive(const ScrollBar::Type orientation)
{
    const bool isHParented = hScrollBar_ && hScrollBar_->isParented();
    const bool isVParented = vScrollBar_ && vScrollBar_->isParented();
    switch (orientation)
    {
        case ScrollBar::Type::HORIZONTAL:
            return isHParented;
        case ScrollBar::Type::VERTICAL:
            return isVParented;
        case ScrollBar::Type::ALL:
            return (isHParented && isVParented);
        case ScrollBar::Type::NONE:
            return !(isHParented || isVParented);
        default:
            log_.errorLn("Invalid orientation!");
    }

    return false;
}

void Box::updateOverflow(const glm::ivec2& overflow)
{
    overflow_ = overflow;

    if (hScrollBar_)
    {
        if (overflow.x > 0 && !hScrollBar_->isParented()) { append(hScrollBar_); }
        else if (overflow.x <= 0 && hScrollBar_->isParented()) { remove(hScrollBar_->getId()); }
        getState()->isLayoutDirty = hScrollBar_->setOverflow(overflow.x);
    }

    if (vScrollBar_)
    {
        if (overflow.y > 0 && !vScrollBar_->isParented()) { append(vScrollBar_); }
        else if (overflow.y <= 0 && vScrollBar_->isParented()) { remove(vScrollBar_->getId()); }
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

void Box::setupReloadables()
{
    layout_.onAllowOverflowChange = [this]()
    {
        if (layout_.allowOverflow.x && !hScrollBar_)
        {
            hScrollBar_ = std::make_shared<ScrollBar>("HBar", ScrollBar::Type::HORIZONTAL);
            if (overflow_.x > 0) { append(hScrollBar_); }
        }
        else if (!layout_.allowOverflow.x && hScrollBar_)
        {
            remove(hScrollBar_->getId());
            hScrollBar_.reset();
        }

        if (layout_.allowOverflow.y && !vScrollBar_)
        {
            vScrollBar_ = std::make_shared<ScrollBar>("VBar", ScrollBar::Type::VERTICAL);
            if (overflow_.x > 0) { append(hScrollBar_); }
        }
        else if (!layout_.allowOverflow.y && vScrollBar_)
        {
            remove(hScrollBar_->getId());
            vScrollBar_.reset();
        }
    };
}

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

glm::vec4 Box::getColor() const { return color_; }

glm::vec4 Box::getBorderColor() const { return borderColor_; }

ScrollBarWPtr Box::getHBar() { return Utils::ref<ScrollBar>(hScrollBar_); }

ScrollBarWPtr Box::getVBar() { return Utils::ref<ScrollBar>(vScrollBar_); }
} // namespace msgui