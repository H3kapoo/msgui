#pragma once

#include "AbstractNode.hpp"
#include "msgui/Listeners.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

namespace msgui
{
class Dropdown;
using DropdownPtr = std::shared_ptr<Dropdown>;

class Dropdown : public AbstractNode
{
public:
    Dropdown(const std::string& name);
    
    template<typename T>
    std::shared_ptr<T> createMenuItem();

    DropdownPtr createSubMenuItem();
    void removeMenuItem(const int32_t idx);
    void disableItem(const int32_t idx);

    void toggleDropdown();

    Dropdown& setColor(const glm::vec4& color);
    Dropdown& setBorderColor(const glm::vec4& color);
    Dropdown& setDropdownOpen(const bool value);

    glm::vec4 getColor() const;
    bool isDropdownOpen() const;
    uint32_t getDropdownId() const;
    Listeners& getListeners();

private:
    void setShaderAttributes() override;

    void onMouseRelease(const nodeevent::LMBRelease&);
    void onFocusLost(const nodeevent::FocusLost&);

    void closeDropdownsOnTheSameLevelAsMe();
    void recursivelyCloseDropdownsUpwards();
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    uint32_t dropdownId_{0};
    Listeners listeners_;

    bool dropdownOpen_{false};
    BoxPtr container_{nullptr};
};
} // namespace msgui