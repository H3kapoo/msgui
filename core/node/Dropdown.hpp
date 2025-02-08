#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"

namespace msgui
{
class Dropdown : public AbstractNode
{
public:
    Dropdown(const std::string& name);

    void addMenuItem(const AbstractNodePtr& nodeItem);

    Dropdown& setColor(const glm::vec4& color);
    Dropdown& setBorderColor(const glm::vec4& color);
    Dropdown& setDropdown(const std::string& DropdownPath);

    glm::vec4 getColor() const;
    Listeners& getListeners();

private:
    void setShaderAttributes() override;

    void onMouseButtonNotify() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    Listeners listeners_;

    bool dropdownOpen_{false};
    // BoxPtr container_{nullptr};
    BoxPtr container_{nullptr};
};
using DropdownPtr = std::shared_ptr<Dropdown>;
} // namespace msgui