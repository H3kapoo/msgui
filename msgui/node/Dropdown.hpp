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
    Dropdown(const std::string& name);

    template<typename T>
    requires (std::is_base_of_v<AbstractNode, T>)
    std::weak_ptr<T> createMenuItem()
    {
        std::shared_ptr<T> nodeItem = Utils::make<T>("DropdownItem");
        nodeItem->getLayout()
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setScale({1.0f, 20});
        nodeItem->getEvents().template listen<nodeevent::LMBRelease, nodeevent::InternalChannel>([this](const auto&)
        {
            /* Close any open downwards and upwards dropdowns from here. */
            setDropdownOpen(false);
            recursivelyCloseDropdownsUpwards();
        });

        container_->append(nodeItem);

        return nodeItem;
    }

    DropdownWPtr createSubMenuItem();
    void removeMenuItem(const int32_t idx);
    void disableItem(const int32_t idx);

    void toggleDropdown();

    Dropdown& setColor(const glm::vec4& color);
    Dropdown& setBorderColor(const glm::vec4& color);
    Dropdown& setDropdownOpen(const bool value);
    Dropdown& setPressedColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    bool isDropdownOpen() const;
    uint32_t getDropdownId() const;

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
    uint32_t dropdownId_{0};
    bool dropdownOpen_{false};
    int32_t shrinkFactor{2};
    BoxPtr container_{nullptr};
};
} // namespace msgui