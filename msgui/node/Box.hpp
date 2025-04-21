#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/events/FocusLost.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/events/LMBRelease.hpp"

namespace msgui
{
class TreeView;
class Dropdown;
using DropdownPtr = std::shared_ptr<Dropdown>;
using DropdownWPtr = std::weak_ptr<Dropdown>;

/* Node used for holding different other nodes. Also provides overflow handling functionality. */
class Box : public AbstractNode
{
public:
    Box(const std::string& name);

    DropdownWPtr createContextMenu();
    void removeContextMenu();

    Box& setColor(const glm::vec4& color);
    Box& setBorderColor(const glm::vec4& color);
    
    bool isScrollBarActive(const utils::Layout::Type type);
    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    SliderWPtr getHBar();
    SliderWPtr getVBar();

    ABSTRACT_NODE_ALLOW_APPEND_REMOVE;

    void setOverflow(const glm::ivec2& overflow);

private:
    Box(const Box&) = delete;
    Box(Box&&) = delete;
    Box& operator=(const Box&) = delete;
    Box& operator=(Box&&) = delete;

    void setShaderAttributes() override;
    void onLMBRelease(const events::LMBRelease& evt);
    void onRMBRelease(const events::RMBRelease& evt);
    void onFocusLost(const events::FocusLost& evt);
    void setupReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::ivec2 overflow_{0, 0};
    SliderPtr vScrollBar_{nullptr};
    SliderPtr hScrollBar_{nullptr};
    AbstractNodePtr ctxMenuFloatingBox_{nullptr};
};
using BoxPtr = std::shared_ptr<Box>;
using BoxWPtr = std::weak_ptr<Box>;
} // namespace msgui