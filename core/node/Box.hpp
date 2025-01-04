#pragma once

#include <memory>

#include "core/Listeners.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/ScrollBar.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class WindowFrame; //Friend
class Box : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color;
    };

public:
    Box(const std::string& name);


    void* getProps() override;
    bool isScrollBarActive(const ScrollBar::Orientation orientation);

private: // friend
    friend WindowFrame;
    void updateOverflow(const glm::ivec2& overflow);

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;
    void setupReloadables();

public:
    Listeners listeners;
    Props props;

private:
    glm::ivec2 overflow_{0, 0}; // temporary here
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui