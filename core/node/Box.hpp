#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
class Box : public AbstractNode
{
public:
    // Internal Defs
    struct Props
    {
        bool isVScrollOn{false};
        bool isHScrollOn{true};
    };

public:
    Box(const std::string& name);

    // Setters
    void setColor(const glm::vec4& color);

    // Temporary
    void enableVScroll()
    {
        log_.infoLn("appending sb");
        if (!vScrollBar_)
        {
            vScrollBar_ = std::make_shared<ScrollBar>(ScrollBar::Orientation::VERTICAL);
            append(vScrollBar_);
        }

        if (!hScrollBar_)
        {
            hScrollBar_ = std::make_shared<ScrollBar>(ScrollBar::Orientation::HORIZONTAL);
            append(hScrollBar_);
        }
    }

    void* getProps() override
    {
        return (void*)&props_;
    }

private:
    // Overrides
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;

private:
    glm::vec4 color_{1.0f};
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
    Props props_;
};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui