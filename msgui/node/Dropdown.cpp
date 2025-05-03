#include "Dropdown.hpp"

#include <GLFW/glfw3.h>

#include "msgui/events/FocusLost.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/Utils.hpp"

namespace msgui
{
Dropdown::Dropdown(const std::string& name) : Button(name)
{
    /* Defaults */
    log_ = Logger("Dropdown(" + name + ")");
    setType(AbstractNode::NodeType::DROPDOWN);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    
    color_ = Utils::hexToVec4("#c7c7c7ff");
    pressedColor_ = Utils::hexToVec4("#dadadaff");
    borderColor_ = Utils::hexToVec4("#D2CCC8ff");
    disabledColor_ = Utils::hexToVec4("#bbbbbbff");
    currentColor_ = color_;

    layout_.setNewScale({70_px, 34_px});

    /* Register only the events you need. */
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&Dropdown::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<events::LMBReleaseNotHovered, events::InputChannel>(
        std::bind(&Dropdown::onMouseReleaseNotHovered, this, std::placeholders::_1));
    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&Dropdown::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::FocusLost, events::InputChannel>(
    std::bind(&Dropdown::onFocusLost, this, std::placeholders::_1));
    // getEvents().listen<events::MouseEnter, events::InputChannel>([](const auto&){});
    // getEvents().listen<events::MouseExit, events::InputChannel>([](const auto&){});

    container_ = Utils::make<Box>("ItemsContainer");
    container_->setType(AbstractNode::NodeType::DROPDOWN_CONTAINTER);
    container_->setColor(Utils::hexToVec4("#ffffffff"));
    container_->getLayout()
        .setType(utils::Layout::Type::VERTICAL)
        .setNewScale({1_fit});

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

void Dropdown::onMouseClick(const events::LMBClick&)
{
    currentColor_ = pressedColor_;

    // layout_.shrink = {2, 2};
    MAKE_LAYOUT_DIRTY;
}

void Dropdown::onMouseRelease(const events::LMBRelease&)
{
    closeDropdownsOnTheSameLevelAsMe();
    toggleDropdown();

    if (isDropdownOpen())
    {
        currentColor_ = pressedColor_;
    }
    else
    {
        currentColor_ = color_;
    }

    layout_.shrink = {0, 0};
    MAKE_LAYOUT_DIRTY;
}

void Dropdown::onMouseReleaseNotHovered(const events::LMBReleaseNotHovered&)
{
    /* In the particular case of receiving the event from Input, LMBReleaseNotHovered acrs just like LMBRelease. */
    const events::LMBRelease evt{{0, 0}};
    onMouseRelease(evt);
}

void Dropdown::onFocusLost(const events::FocusLost&)
{
    if (!dropdownOpen_) { return; }

    const FrameStatePtr& state = getState();
    const AbstractNodePtr& parentBoxCont = state->clickedNodePtr.lock()->getParent().lock();
    const AbstractNodePtr& grandParentDd = parentBoxCont ? parentBoxCont->getParent().lock() : nullptr;
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
    subMenu->getLayout().setNewScale(itemSize_);
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

/*
    Look into the container containing this dropdown and close the other dropdowns except this one.
    There shall be only one open at most at any given time, so break out quickly.
*/
void Dropdown::closeDropdownsOnTheSameLevelAsMe()
{
    const AbstractNodePtr& parent = getParent().lock();
    if (!parent) { return; }

    for (const AbstractNodePtr& subItem : parent->getChildren())
    {
        if (subItem->getType() == AbstractNode::NodeType::DROPDOWN && subItem->getId() != getId())
        {
            Utils::as<Dropdown>(subItem)->setDropdownOpen(false);
            break;
        }
    }
}

/*
    Get the grandparent of this node and check if it's a dropdown as well. Close it and then propagate
    further up until the node is no longer a dropdown node to be closed.
    We get the grandparent and not the parent because of the Box container inside of the dropdown that
    actually holds the menu items.
*/
void Dropdown::recursivelyCloseDropdownsUpwards()
{
    AbstractNodeWPtr parent = getParent();
    while (parent.lock() != nullptr)
    {
        const AbstractNodePtr myParent = parent.lock();
        const AbstractNodePtr myParentsParent = myParent ? myParent->getParent().lock() : nullptr;
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

Dropdown& Dropdown::setDropdownOpen(const bool value)
{
    dropdownOpen_ = value;

    /* Dropdown shall close. If any submenus are open, close them. */
    if (!dropdownOpen_)
    {
        /* If other dropdowns are open underneath this, try to close them also. */
        for (const AbstractNodePtr& subItem : container_->getChildren())
        {
            if (subItem->getType() == AbstractNode::NodeType::DROPDOWN)
            {
                Utils::as<Dropdown>(subItem)->setDropdownOpen(false);
            }
        }

        remove(container_->getId());
    }
    /* Dropdown shall open. */
    else
    {
        if (!container_->isParented())
        {
            appendAt(container_, 0);
        }
    }

    return *this;
}

Dropdown& Dropdown::setPressedColor(const glm::vec4& color)
{
    pressedColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Dropdown& Dropdown::setItemSize(const Layout::ScaleXY& size)
{
    itemSize_ = size;

    if (getState())
    {
        for (auto& ch : container_->getChildren())
        {
            ch->getLayout().setNewScale(itemSize_);
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

bool Dropdown::isDropdownOpen() const { return dropdownOpen_; }

uint32_t Dropdown::getDropdownId() const { return dropdownId_; }

BoxWPtr Dropdown::getContainer() { return container_; }

Layout::ScaleXY Dropdown::getItemSize() { return itemSize_; }

Dropdown::Expand Dropdown::getExpandDirection() const { return expandDir_; }
} // msgui