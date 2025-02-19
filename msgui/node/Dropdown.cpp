#include "Dropdown.hpp"

#include <cstdint>
#include <functional>

#include <GLFW/glfw3.h>

#include "msgui/MeshLoader.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/NodeEventManager.hpp"

namespace msgui
{
Dropdown::Dropdown(const std::string& name) : AbstractNode(name, NodeType::DROPDOWN)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Dropdown(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");
    pressedColor_ = Utils::hexToVec4("#dadada");
    borderColor_ = Utils::hexToVec4("#D2CCC8");
    disabledColor_ = Utils::hexToVec4("#bbbbbbff");
    currentColor_ = color_;
    itemSize_ = {70, 34};

    layout_.setBorder({1});
    layout_.setBorderRadius({4});
    layout_.setScale({70, 34});

    /* Register only the events you need. */
    getEvents().listen<nodeevent::LMBRelease, nodeevent::InputChannel>(
        std::bind(&Dropdown::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<nodeevent::LMBClick, nodeevent::InputChannel>(
        std::bind(&Dropdown::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<nodeevent::FocusLost, nodeevent::InputChannel>(
        std::bind(&Dropdown::onFocusLost, this, std::placeholders::_1));

    container_ = Utils::make<Box>("ItemsContainer");
    container_->setColor(Utils::hexToVec4("#4aabeb00"));
    container_->getLayout().setType(Layout::Type::VERTICAL);

    dropdownId_ = getId();
}

void Dropdown::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();

    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", currentColor_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Dropdown::onMouseClick(const nodeevent::LMBClick&)
{
    currentColor_ = pressedColor_;

    transform_.pos.x += shrinkFactor;
    transform_.pos.y += shrinkFactor;
    transform_.scale.x -= shrinkFactor*2;
    transform_.scale.y -= shrinkFactor*2;
}

void Dropdown::onMouseRelease(const nodeevent::LMBRelease&)
{
    closeDropdownsOnTheSameLevelAsMe();
    toggleDropdown();

    currentColor_ = color_;

    transform_.pos.x -= shrinkFactor;
    transform_.pos.y -= shrinkFactor;
    transform_.scale.x += shrinkFactor*2;
    transform_.scale.y += shrinkFactor*2;
}

void Dropdown::onFocusLost(const nodeevent::FocusLost&)
{
    const auto& state = getState();
    const auto& parentBoxCont = state->clickedNodePtr->getParent().lock();
    const auto& grandParentDd = parentBoxCont ? parentBoxCont->getParent().lock() : nullptr;
    if (!grandParentDd || grandParentDd->getType() != AbstractNode::NodeType::DROPDOWN ||
        Utils::as<Dropdown>(grandParentDd)->getDropdownId() != dropdownId_)
    {
        recursivelyCloseDropdownsUpwards();
        setDropdownOpen(false);
    }
}

DropdownWPtr Dropdown::createSubMenuItem()
{
    DropdownPtr subMenu = Utils::make<Dropdown>("SubDropdown");
    subMenu->setColor(color_);
    subMenu->getLayout()
        .setScaleType(Layout::ScaleType::ABS)
        .setScale(itemSize_);
    subMenu->dropdownId_ = dropdownId_;
    container_->append(subMenu);

    return subMenu;
}

void Dropdown::removeMenuItemIdx(const int32_t idx)
{
    container_->removeAt(idx);
}

void Dropdown::removeMenuItemByName(const std::string& name)
{
    container_->removeBy([name](const auto& node) -> bool { return node->getName() == name; });
}

void Dropdown::toggleDropdown()
{
    dropdownOpen_ = !dropdownOpen_;
    setDropdownOpen(dropdownOpen_);
}

void Dropdown::closeDropdownsOnTheSameLevelAsMe()
{
    const auto& parent = getParent().lock();
    if (!parent) { return; }

    /* Look into the container containing this dropdown and close the other dropdowns except this one.
       It shall be only one open at most at any given time, so break out quickly. */
    for (const auto& ch : parent->getChildren())
    {
        if (ch->getType() == AbstractNode::NodeType::DROPDOWN && ch->getId() != getId())
        {
            Utils::as<Dropdown>(ch)->setDropdownOpen(false);
            break;
        }
    }
}

void Dropdown::recursivelyCloseDropdownsUpwards()
{
    /* Get the grandparent of this node and check if it's a dropdown as well. Close it and then propagate
       further up until the node is no longer a dropdown node to be closed.
       We get the grandparent and not the parent because of the Box container inside of the dropdown that
       actually holds the menu items.*/
    auto parent = getParent();
    while (parent.lock() != nullptr)
    {
        const auto myParent = parent.lock();
        const auto myParentsParent = myParent ? myParent->getParent().lock() : nullptr;
        if (myParentsParent && myParentsParent->getType() == AbstractNode::NodeType::DROPDOWN)
        {
            Utils::as<Dropdown>(myParentsParent)->setDropdownOpen(false);
            parent = myParentsParent->getParent();
        }
        else
        {
            break;
        }
    }
}

void Dropdown::setupLayoutReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
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
}

Dropdown& Dropdown::setColor(const glm::vec4& color)
{
    color_ = color;
    currentColor_ = color;
    REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setBorderColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setDropdownOpen(const bool value)
{
    dropdownOpen_ = value;

    /* Dropdown shall close. Reset scrollbar knob pos and if any submenus are open, close them. */
    if (!dropdownOpen_ && getChildren().size())
    {
        /* If other dropdowns are open underneath me, try to close them also. */
        for (const auto& ch : container_->getChildren())
        {
            if (ch->getType() == AbstractNode::NodeType::DROPDOWN)
            {
                Utils::as<Dropdown>(ch)->setDropdownOpen(false);
            }
        }

        remove(container_->getId());
    }
    /* Dropdown shall open. */
    else if (dropdownOpen_ && !getChildren().size())
    {
        append(container_);
    }

    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setPressedColor(const glm::vec4& color)
{
    pressedColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Dropdown& Dropdown::setItemSize(const glm::ivec2& size)
{
    itemSize_ = size;

    if (getState())
    {
        for (auto& ch : container_->getChildren())
        {
            ch->getLayout().setScale(itemSize_);
        }
    }

    return *this;
}

Dropdown& Dropdown::setExpandDirection(const Expand expand)
{
    expandDir_ = expand;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 Dropdown::getColor() const { return color_; }

bool Dropdown::isDropdownOpen() const { return dropdownOpen_; }

uint32_t Dropdown::getDropdownId() const { return dropdownId_; }

BoxWPtr Dropdown::getContainer() { return container_; }

glm::ivec2 Dropdown::getItemSize() { return itemSize_; }

Dropdown::Expand Dropdown::getExpandDirection() const { return expandDir_; }
} // msgui