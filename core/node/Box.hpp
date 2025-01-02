#pragma once

#include <memory>

#include "core/Listeners.hpp"
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

    void* getProps() override;

    // Setters
    void setColor(const glm::vec4& color);

    // Getters
    Listeners& getListeners();

    // Temporary
    void enableVScroll()
    {
        log_.infoLn("appending sb");
        if (!vScrollBar_)
        {
            props_.isVScrollOn = true;
            vScrollBar_ = std::make_shared<ScrollBar>(log_.getName(), ScrollBar::Orientation::VERTICAL);
            append(vScrollBar_);
        }

        if (!hScrollBar_)
        {
            props_.isHScrollOn = true;
            hScrollBar_ = std::make_shared<ScrollBar>(log_.getName(), ScrollBar::Orientation::HORIZONTAL);
            append(hScrollBar_);
        }
    }

private:
    // Overrides
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;

private:
    glm::vec4 color_{1.0f};
    ScrollBarPtr vScrollBar_{nullptr};
    ScrollBarPtr hScrollBar_{nullptr};
    Listeners listeners_;
    Props props_;
};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui