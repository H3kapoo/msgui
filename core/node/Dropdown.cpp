#include "Dropdown.hpp"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <functional>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
Dropdown::Dropdown(const std::string& name) : AbstractNode(name, NodeType::DROPDOWN)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Dropdown(" + name + ")");

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#ffffffff");

    // container_ = Utils::make<Box>("BoxIn");
    container_ = Utils::make<Box>("BoxIn");
    container_->setColor(Utils::hexToVec4("#48ff00ff"));
    container_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({false, true})
        // .setScaleType(Layout::ScaleType::ABS)
        // .setScale({100, 100})
        .setPadding(Layout::TBLR{0, 0, 2, 2})
        // .setMargin({10, 10, 60, 10})
        ;

    container_->getListeners().setOnMouseButtonLeftClick([this]()
    {
        log_.debugLn("clicked box");
    });

    rootId_ = getId();
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

void Dropdown::onMouseButtonNotify()
{
    const auto& state = getState();
    int32_t clicked = state->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT];

    /* Close any other open dropdowns on the same level as this one. */
    closeDropdownsOnTheSameLevelAsMe();

    /* On mouse release if we are still the hovered button, toggle dropdown. */
    if (!clicked && this == state->hoveredNodePtr.get())
    {
        toggleDropdown();
    }

    /* Mouse clicked elsewhere. This case will happen when we clicked elsewhere but we need to notify
       the previous element of this so it can do some cleanup. In this case, we need to close
       everything that has to do with this dropdown, all the way up and down. */
    if (clicked && this == state->prevClickedNodePtr.get())
    {
        const auto& parent =  state->clickedNodePtr->getParent().lock();
        const auto& grandParent = parent ? parent->getParent().lock() : nullptr;
        if (state->clickedNodePtr->getType() == AbstractNode::NodeType::DROPDOWN &&
            Utils::as<Dropdown>(state->clickedNodePtr)->rootId_ == rootId_)
        {
            // bool fromSameMasterDd = Utils::as<Dropdown>(state->clickedNodePtr)->rootId_ == rootId_;
            // log_.debugLn("from same dd %d", fromSameMasterDd);
        }
        else if (grandParent && grandParent->getType() == AbstractNode::NodeType::DROPDOWN &&
            Utils::as<Dropdown>(grandParent)->rootId_ == rootId_)
        {

        }
        else
        {
            log_.debugLn("elseee");
            recursivelyCloseDropdownsUpwards();
            setDropdownOpen(false);
        }
    }
}

void Dropdown::addMenuItem(const glm::vec4& itemData, const std::function<void()> callback)
{
    ButtonPtr nodeItem = Utils::make<Button>("Btn");
    nodeItem->setColor(itemData);
    nodeItem->getLayout()
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        .setScale({1.0f, 20})
        // .setMargin({1, 1, 0, 0})
        ;
    nodeItem->getListeners().setOnMouseButtonLeftClick([this, callback]()
    {
        /* Close any open downwards and upwards dropdowns from here. */
        setDropdownOpen(false);
        recursivelyCloseDropdownsUpwards();
        callback();
    });

    container_->append(nodeItem);
}

void Dropdown::addMenuItem(const DropdownPtr& nodeItem)
{
    nodeItem->rootId_ = rootId_;
    container_->append(nodeItem);
}

void Dropdown::toggleDropdown()
{
    dropdownOpen_ = !dropdownOpen_;
    setDropdownOpen(dropdownOpen_);
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

Listeners& Dropdown::getListeners() { return listeners_; }
} // msgui