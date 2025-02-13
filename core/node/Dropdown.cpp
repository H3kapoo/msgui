#include "Dropdown.hpp"

#include <cstdint>
#include <functional>

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/ShaderLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/Image.hpp"
#include "core/nodeEvent/LMBClick.hpp"
#include "core/nodeEvent/LMBRelease.hpp"
#include "core/nodeEvent/NodeEventManager.hpp"

namespace msgui
{
Dropdown::Dropdown(const std::string& name) : AbstractNode(name, NodeType::DROPDOWN)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Dropdown(" + name + ")");

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#ffffffff");

    container_ = Utils::make<Box>("BoxIn");
    container_->setColor(Utils::hexToVec4("#48ff00ff"));
    container_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({false, true})
        .setPadding(Layout::TBLR{0, 0, 2, 2})
        ;

    dropdownId_ = getId();

    /* Register only the events you need. */
    getEvents().listen<nodeevent::LMBRelease, InputChannel>(
        std::bind(&Dropdown::onMouseRelease, this, std::placeholders::_1));
    getEvents().listen<nodeevent::FocusLost, InputChannel>(
        std::bind(&Dropdown::onFocusLost, this, std::placeholders::_1));
}

void Dropdown::setShaderAttributes()
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

void Dropdown::onMouseRelease(const nodeevent::LMBRelease&)
{
    closeDropdownsOnTheSameLevelAsMe();
    toggleDropdown();
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

template<typename T>
std::shared_ptr<T> Dropdown::createMenuItem()
{
    std::shared_ptr<T> nodeItem = Utils::make<T>("DropdownItem");
    nodeItem->getLayout()
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        .setScale({1.0f, 20});
    nodeItem->getEvents().template listen<LMBRelease, InternalChannel>([this](const auto&)
    {
        /* Close any open downwards and upwards dropdowns from here. */
        setDropdownOpen(false);
        recursivelyCloseDropdownsUpwards();
    });

    container_->append(nodeItem);

    return nodeItem;
}

/* Currently only button and image can be a menu item (except another dropdown). */
template ButtonPtr Dropdown::createMenuItem<Button>();
template ImagePtr Dropdown::createMenuItem<Image>();

DropdownPtr Dropdown::createSubMenuItem()
{
    DropdownPtr subMenu = Utils::make<Dropdown>("Drop");
    subMenu->setColor(Utils::hexToVec4("#ffaa00ff"));
    subMenu->getLayout()
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        .setScale({1.0f, 20});
    subMenu->dropdownId_ = dropdownId_;
    container_->append(subMenu);

    return subMenu;
}

void Dropdown::removeMenuItem(const int32_t idx)
{
    container_->removeAt(idx);
}

void Dropdown::disableItem(const int32_t idx)
{
    if (idx < 0 || idx > (int32_t)container_->getChildren().size() - 1) { return; }
    Utils::as<Button>(container_->getChildren()[idx])->setEnabled(false);
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

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

Dropdown& Dropdown::setColor(const glm::vec4& color)
{
    color_ = color;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setBorderColor(const glm::vec4& color)
{
    color_ = color;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setDropdownOpen(const bool value)
{
    dropdownOpen_ = value;

    /* Dropdown shall close. Reset scrollbar knob pos and if any submenus are open, close them. */
    if (!dropdownOpen_ && getChildren().size())
    {
        // if (container_->isScrollBarActive(ScrollBar::Type::VERTICAL))
        // {
        //     container_->getScrollBar(ScrollBar::Type::VERTICAL)->setScrollCurrentValue(0);
        // }

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

glm::vec4 Dropdown::getColor() const { return color_; }

bool Dropdown::isDropdownOpen() const { return dropdownOpen_; }

uint32_t Dropdown::getDropdownId() const { return dropdownId_; }

Listeners& Dropdown::getListeners() { return listeners_; }
} // msgui