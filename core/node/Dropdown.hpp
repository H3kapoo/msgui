#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/Button.hpp"

namespace msgui
{
class Dropdown;
using DropdownPtr = std::shared_ptr<Dropdown>;

class Dropdown : public AbstractNode
{
public:
    Dropdown(const std::string& name);

    void addMenuItem(const glm::vec4& itemData, const std::function<void()> callback);
    void addMenuItem(const DropdownPtr& nodeItem);

    // void addMenuItem(const ButtonPtr& nodeItem);
    void toggleDropdown();

    Dropdown& setColor(const glm::vec4& color);
    Dropdown& setBorderColor(const glm::vec4& color);
    Dropdown& setDropdownOpen(const bool value);

    glm::vec4 getColor() const;
    bool isDropdownOpen() const;
    Listeners& getListeners();

    uint32_t rootId_{0};
private:
    void closeDropdownsOnTheSameLevelAsMe();
    void recursivelyCloseDropdownsUpwards();
    void setShaderAttributes() override;

    void onMouseButtonNotify() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    Listeners listeners_;

    bool dropdownOpen_{false};
    BoxPtr container_{nullptr};
};
} // namespace msgui