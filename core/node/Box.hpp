#pragma once

#include "core/Listeners.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
class Box : public AbstractNode
{
struct Props;
public:
    Box(const std::string& name);

    bool isScrollBarActive(const ScrollBar::Orientation orientation);

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);
    Props& setScrollbarSize(const int32_t size);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getScrollbarSize() const;

private: // friend
    friend WindowFrame;
    void updateOverflow(const glm::ivec2& overflow);

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;
    void onMouseDragNotify() override;

public:
    Listeners listeners;

private:
    struct Props
    {
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
        int32_t scrollBarSize{20};
    };

    Props props;
    glm::ivec2 overflow_{0, 0};
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui