#pragma once

#include "msgui/events/FocusLost.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"

namespace msgui
{
class Dropdown;
using DropdownPtr = std::shared_ptr<Dropdown>;
using DropdownWPtr = std::weak_ptr<Dropdown>;

/* Node used to display and handle dropdown menu and submenu items. */
class Dropdown : public Button
{
public:
    enum class Expand : uint8_t { LEFT, RIGHT, TOP, BOTTOM };

public:
    Dropdown(const std::string& name);

    /**
        Creates a new menu item of the template type specified by the user.
        Pointer is owned by the dropdown class so just a weak handle will be returned.

        @param T Type of the item to be added

        @return T type weak pointer to the new item added
    */
    template<typename T>
    requires (std::is_base_of_v<AbstractNode, T>)
    std::weak_ptr<T> createMenuItem()
    {
        std::shared_ptr<T> nodeItem = Utils::make<T>("DropdownItem");
        nodeItem->getLayout().setNewScale(itemSize_);
        nodeItem->getEvents().template listen<events::LMBRelease, events::InternalChannel>([this](const auto&)
        {
            /* Close any open downwards and upwards dropdowns from here. */
            setDropdownOpen(false);
            recursivelyCloseDropdownsUpwards();
        });

        nodeItem->getEvents().template listen<events::FocusLost, events::InternalChannel>([this](const auto& evt)
        {
            /* This is mainly used when the menu item has it's events disabled and we click somewhere else.
               We need to notify the parent to run the close dropdown logic only when dropdown is still open. */
            if (dropdownOpen_) { onFocusLost(evt); }
        });

        container_->append(nodeItem);

        return nodeItem;
    }

    /**
        Creates a new sub menu item of the Dropdown type.
        Pointer is owned by the dropdown class so just a weak handle will be returned.

        @return T type weak pointer to the new dropdown type added
    */
    DropdownWPtr createSubMenuItem();

    /**
        Remove an item from the menu at a specific index.

        @param idx Index of the item to be removed
    */
    void removeMenuItemIdx(const int32_t idx);

    /**
        Remove an item from the menu that has a specific name.

        @param name Name of the item to be removed
    */
    void removeMenuItemByName(const std::string& name);

    /**
        Simply toggle on or off the dropdown. Open or closed.
    */
    void toggleDropdown();

    Dropdown& setDropdownOpen(const bool value);
    Dropdown& setPressedColor(const glm::vec4& color);
    Dropdown& setItemSize(const Layout::ScaleXY& size);
    Dropdown& setExpandDirection(const Expand expand);

    bool isDropdownOpen() const;
    uint32_t getDropdownId() const;
    BoxWPtr getContainer();
    Layout::ScaleXY getItemSize();
    Expand getExpandDirection() const;

private:
    /* Can't be copied or moved. */
    Dropdown(const Dropdown&) = delete;
    Dropdown(Dropdown&&) = delete;
    Dropdown& operator=(const Dropdown&) = delete;
    Dropdown& operator=(Dropdown&&) = delete;

    void setShaderAttributes() override;
    void onMouseRelease(const events::LMBRelease&);
    void onMouseReleaseNotHovered(const events::LMBReleaseNotHovered&);
    void onMouseClick(const events::LMBClick&);
    void onFocusLost(const events::FocusLost&);

    void closeDropdownsOnTheSameLevelAsMe();
    void recursivelyCloseDropdownsUpwards();

private:
    Layout::ScaleXY itemSize_{70_px, 34_px};
    Expand expandDir_{Expand::BOTTOM};
    uint32_t dropdownId_{0};
    bool dropdownOpen_{false};
    BoxPtr container_{nullptr};
};
} // namespace msgui