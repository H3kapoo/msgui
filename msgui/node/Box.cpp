#include "Box.hpp"

#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/events/FocusLost.hpp"

namespace msgui
{
Box::Box(const std::string& name)
    : AbstractNode("Box(" + name +")", NodeType::BOX)
{
    log_ = Logger(getName());
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7FF");

    layout_.setNewScale({200_px, 100_px});

    setupReloadables();

    /* Register only the events you need. */
    getEvents().listen<events::RMBRelease, events::InputChannel>(
        std::bind(&Box::onRMBRelease, this, std::placeholders::_1));
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&Box::onLMBRelease, this, std::placeholders::_1));
    getEvents().listen<events::FocusLost, events::InputChannel>(
        std::bind(&Box::onFocusLost, this, std::placeholders::_1));
}

DropdownWPtr Box::createContextMenu()
{
    if (!ctxMenuFloatingBox_)
    {
        DropdownPtr ctxDd = Utils::make<Dropdown>("ContextMenuDropdown");
        ctxDd->getLayout().setNewScale({0_px, 0_px});

        auto fb = Utils::make<FloatingBox>("ContextMenuFloatingBox");
        fb->getLayout().setNewScale({0_px, 0_px});
        fb->append(ctxDd);
        ctxMenuFloatingBox_ = fb;
        append(ctxMenuFloatingBox_);
    }
    return Utils::as<Dropdown>(ctxMenuFloatingBox_->getChildren()[0]);
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

void Box::onLMBRelease(const events::LMBRelease& ev)
{
    /* Nothing to be done if no context menu is assigned. */
    if (!ctxMenuFloatingBox_) { return; }
    
    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    fb->setPreferredPosition(ev.pos);

    auto& ddd = fb->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(false);
}

void Box::onFocusLost(const events::FocusLost&)
{
    /* Nothing to be done if no context menu is assigned or if focus is lost
       BUT the newly clicked node is a drop item (the menu's one most likely). */
    if (!ctxMenuFloatingBox_) { return; }
    if (getState()->clickedNodePtr.lock()->getName() == "DropdownItem"
        || getState()->clickedNodePtr.lock()->getName() == "SubDropdown")
    {
        return;
    }

    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    auto& ddd = fb->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(false);
}

void Box::onRMBRelease(const events::RMBRelease& evt)
{
    if (!ctxMenuFloatingBox_) { return; }

    FloatingBoxPtr fb = Utils::as<FloatingBox>(ctxMenuFloatingBox_);
    fb->setPreferredPosition(evt.pos);
    auto& ddd = fb->getChildren()[0];
    Utils::as<Dropdown>(ddd)->setDropdownOpen(true);
}

bool Box::isScrollBarActive(const utils::Layout::Type type)
{
    if (type == utils::Layout::Type::HORIZONTAL)
    {
        return hScrollBar_ && hScrollBar_->isParented();
    }
    else if (type == utils::Layout::Type::VERTICAL)
    {
        return vScrollBar_ && vScrollBar_->isParented();
    }

    return false;
}

void Box::setOverflow(const glm::ivec2& overflow)
{
    overflow_ = overflow;

    if (hScrollBar_)
    {
        if (overflow.x > 0 && !hScrollBar_->isParented())
        {
            append(hScrollBar_);
        }
        else if (overflow.x <= 0 && hScrollBar_->isParented())
        {
            hScrollBar_->setSlideCurrentValue(0);
            remove(hScrollBar_->getId());
        }

        if (!(getState()->layoutPassActions & ELayoutPass::RECALCULATE_NODE_TRANSFORM)
            && (int32_t)hScrollBar_->getSlideTo() != overflow.x)
        {
            hScrollBar_->setSlideTo(overflow.x);
        }
    }

    if (vScrollBar_)
    {
        if (overflow.y > 0 && !vScrollBar_->isParented())
        {
            append(vScrollBar_);
        }
        else if (overflow.y <= 0 && vScrollBar_->isParented())
        {
            vScrollBar_->setSlideCurrentValue(0);
            remove(vScrollBar_->getId());
        }

        if (!(getState()->layoutPassActions & ELayoutPass::RECALCULATE_NODE_TRANSFORM)
            && (int32_t)vScrollBar_->getSlideTo() != overflow.y)
        {
            vScrollBar_->setSlideTo(overflow.y);
        }
    }
}

void Box::setupReloadables()
{
    layout_.onAllowOverflowChange = [this]()
    {
        if (layout_.allowOverflow.x && !hScrollBar_)
        {

            hScrollBar_ = std::make_shared<Slider>("HSlider");
            hScrollBar_->enableViewValue(false).enableDynamicKnob(true);
            hScrollBar_->setType(AbstractNode::NodeType::SCROLL);
            hScrollBar_->getLayout().setNewScale({1.0_rel, 20_px});
            hScrollBar_->getKnob().lock()->setType(AbstractNode::NodeType::SCROLL_KNOB);

            if (overflow_.x > 0) { append(hScrollBar_); }
        }
        else if (!layout_.allowOverflow.x && hScrollBar_)
        {
            remove(hScrollBar_->getId());
            hScrollBar_.reset();
        }

        if (layout_.allowOverflow.y && !vScrollBar_)
        {
            vScrollBar_ = std::make_shared<Slider>("VSlider");
            vScrollBar_->enableDynamicKnob(true);
            vScrollBar_->getLayout()
                .setType(utils::Layout::Type::VERTICAL)
                .setNewScale({20_px, 1.0_rel});
            vScrollBar_->setType(AbstractNode::NodeType::SCROLL);
            vScrollBar_->getKnob().lock()->setType(AbstractNode::NodeType::SCROLL_KNOB);

            if (overflow_.x > 0) { append(hScrollBar_); }
        }
        else if (!layout_.allowOverflow.y && vScrollBar_)
        {
            remove(hScrollBar_->getId());
            vScrollBar_.reset();
        }
    };
}

// TODO: Things like colors shall be in abstract base class.
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

SliderWPtr Box::getHBar() { return hScrollBar_; }

SliderWPtr Box::getVBar() { return vScrollBar_; }
} // namespace msgui