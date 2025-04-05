#include "Box.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/utils/ScrollBar.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"

namespace msgui
{
Box::Box(const std::string& name) : AbstractNode(name, NodeType::BOX)
{
    log_ = Logger("Box(" + name +")");
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setScale({200, 100});

    setupReloadables();

    /* Register only the events you need. */
    getEvents().listen<nodeevent::RMBRelease, nodeevent::InputChannel>(
        std::bind(&Box::onRMBRelease, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBRelease, nodeevent::InputChannel>(
        std::bind(&Box::onLMBRelease, this, std::placeholders::_1));
    getEvents().listen<nodeevent::FocusLost, nodeevent::InputChannel>(
        std::bind(&Box::onFocusLost, this, std::placeholders::_1));
}

DropdownWPtr Box::createContextMenu()
{
    if (!ctxMenuFloatingBox_)
    {
        DropdownPtr ctxDd = Utils::make<Dropdown>("ContextMenuDropdown");
        ctxDd->getLayout().setScale({0, 0});

        auto fb = Utils::make<FloatingBox>("ContextMenuFloatingBox");
        fb->getLayout().setScale({0, 0});
        fb->getContainer().lock()->append(ctxDd);
        ctxMenuFloatingBox_ = fb;
        append(ctxMenuFloatingBox_);
    }

    return Utils::as<Dropdown>(Utils::as<FloatingBox>(ctxMenuFloatingBox_)
        ->getContainer().lock()->getChildren()[0]);
}

void Box::removeContextMenu()
{
    if (ctxMenuFloatingBox_)
    {
        remove(ctxMenuFloatingBox_->getId());
        ctxMenuFloatingBox_.reset();
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

void Box::onLMBRelease(const nodeevent::LMBRelease&)
{
    /* Nothing to be done if no context menu is assigned. */
    if (!ctxMenuFloatingBox_) { return; }
    
    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    auto& ddd = fb->getContainer().lock()->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(false);
}

void Box::onFocusLost(const nodeevent::FocusLost&)
{
    /* Nothing to be done if no context menu is assigned or if focus is lost
       BUT the newly clicked node is a drop item (the menu's one most likely). */
    if (!ctxMenuFloatingBox_) { return; }
    if (getState()->clickedNodePtr.lock()->getName() == "DropdownItem") { return; }

    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    auto& ddd = fb->getContainer().lock()->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(false);
}

void Box::onRMBRelease(const nodeevent::RMBRelease& evt)
{
    if (!ctxMenuFloatingBox_) { return; }

    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    fb->setPreferredPosition(evt.pos);
    auto& ddd = fb->getContainer().lock()->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(true);
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

        // if (!getState()->isLayoutDirty) { getState()->isLayoutDirty = hScrollBar_->setOverflow(overflow.x); };
        if (!(getState()->layoutPassActions & ELayoutPass::RECALCULATE_NODE_TRANSFORM) && hScrollBar_->setOverflow(overflow.x))
        {
            getState()->layoutPassActions |= ELayoutPass::RECALCULATE_NODE_TRANSFORM;
        }
    }

    if (vScrollBar_)
    {
        if (overflow.y > 0 && !vScrollBar_->isParented()) { append(vScrollBar_); }
        else if (overflow.y <= 0 && vScrollBar_->isParented()) { remove(vScrollBar_->getId()); }

        // if (!getState()->isLayoutDirty) { getState()->isLayoutDirty = vScrollBar_->setOverflow(overflow.y); };
        if (!(getState()->layoutPassActions & ELayoutPass::RECALCULATE_NODE_TRANSFORM) && vScrollBar_->setOverflow(overflow.y))
        {
            getState()->layoutPassActions |= ELayoutPass::RECALCULATE_NODE_TRANSFORM;
        }
    }
}

void Box::setupReloadables()
{
    auto updateCb = [this](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onAlignChildChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
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