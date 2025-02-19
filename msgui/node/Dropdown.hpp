#pragma once

#include "AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

namespace msgui
{
class Dropdown;
using DropdownPtr = std::shared_ptr<Dropdown>;
using DropdownWPtr = std::weak_ptr<Dropdown>;

class Dropdown : public AbstractNode
{
public:
    enum class Expand : uint8_t { LEFT, RIGHT, TOP, BOTTOM };

public:
    Dropdown(const std::string& name);

    template<typename T>
    requires (std::is_base_of_v<AbstractNode, T>)
    std::weak_ptr<T> createMenuItem()
    {
        std::shared_ptr<T> nodeItem = Utils::make<T>("DropdownItem");
        nodeItem->getLayout()
            .setScaleType(Layout::ScaleType::ABS)
            .setScale(itemSize_);
        nodeItem->getEvents().template listen<nodeevent::LMBRelease, nodeevent::InternalChannel>([this](const auto&)
        {
            /* Close any open downwards and upwards dropdowns from here. */
            setDropdownOpen(false);
            recursivelyCloseDropdownsUpwards();
        });

        nodeItem->getEvents().template listen<nodeevent::FocusLost, nodeevent::InternalChannel>([this](const auto& evt)
        {
            /* This is mainly used when the menu item has it's events disabled and we click somewhere else.
               We need to notify the parent to run the close dropdown logic only when dropdown is still open. */
            if (dropdownOpen_) { onFocusLost(evt); }
        });

        container_->append(nodeItem);

        return nodeItem;
    }

    DropdownWPtr createSubMenuItem();
    void removeMenuItemIdx(const int32_t idx);
    void removeMenuItemByName(const std::string& name);
    void toggleDropdown();

    Dropdown& setColor(const glm::vec4& color);
    Dropdown& setBorderColor(const glm::vec4& color);
    Dropdown& setDropdownOpen(const bool value);
    Dropdown& setPressedColor(const glm::vec4& color);
    Dropdown& setItemSize(const glm::ivec2& size);
    Dropdown& setExpandDirection(const Expand expand);

    glm::vec4 getColor() const;
    bool isDropdownOpen() const;
    uint32_t getDropdownId() const;
    BoxWPtr getContainer();
    glm::ivec2 getItemSize();
    Expand getExpandDirection() const;

private:
    void setShaderAttributes() override;

    void onMouseRelease(const nodeevent::LMBRelease&);
    void onMouseClick(const nodeevent::LMBClick&);
    void onFocusLost(const nodeevent::FocusLost&);

    void closeDropdownsOnTheSameLevelAsMe();
    void recursivelyCloseDropdownsUpwards();
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 currentColor_{1.0f};
    glm::vec4 pressedColor_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::vec4 disabledColor_{1.0f};
    glm::ivec2 itemSize_{0};
    Expand expandDir_{Expand::BOTTOM};
    uint32_t dropdownId_{0};
    bool dropdownOpen_{false};
    int32_t shrinkFactor{2};
    BoxPtr container_{nullptr};
};
} // namespace msgui