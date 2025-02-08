#pragma once

#include "core/Listeners.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
class Box : public AbstractNode
{
public:
    Box(const std::string& name);

    bool isScrollBarActive(const ScrollBar::Orientation orientation);

    Box& setColor(const glm::vec4& color);
    Box& setBorderColor(const glm::vec4& color);
    Box& setScrollbarSize(const int32_t size);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getScrollbarSize() const;
    ScrollBarPtr getScrollBar(const ScrollBar::Orientation orientation);
    Listeners& getListeners();

private: // friend
    friend WindowFrame;
    void updateOverflow(const glm::ivec2& overflow);

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;
    void onMouseDragNotify() override;

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    int32_t scrollBarSize_{20};
    glm::ivec2 overflow_{0, 0};
    Listeners listeners_;
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui