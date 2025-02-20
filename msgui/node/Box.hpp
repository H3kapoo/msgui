#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/utils/ScrollBar.hpp"

namespace msgui
{
/* Node used for holding different other nodes. Also provides overflow handling functionality. */
class Box : public AbstractNode
{
public:
    Box(const std::string& name);

    bool isScrollBarActive(const ScrollBar::Type orientation);
    Box& setColor(const glm::vec4& color);
    Box& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    ScrollBarWPtr getHBar();
    ScrollBarWPtr getVBar();

    ABSTRACT_NODE_ALLOW_APPEND_REMOVE;

private: // friend
    friend WindowFrame;
    void updateOverflow(const glm::ivec2& overflow);

private:
    Box(const Box&) = delete;
    Box(Box&&) = delete;
    Box& operator=(const Box&) = delete;
    Box& operator=(Box&&) = delete;

    void setShaderAttributes() override;
    void setupReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::ivec2 overflow_{0, 0};
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
};
using BoxPtr = std::shared_ptr<Box>;
using BoxWPtr = std::weak_ptr<Box>;
} // namespace msgui