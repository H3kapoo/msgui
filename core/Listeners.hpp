#pragma once

#include <cstdint>
#include <functional>

namespace msgui
{
/* Stores callback functions for each node */
class Listeners
{
using MouseButtonCallback = std::function<void(int32_t btn, int32_t action, int32_t x, int32_t y)>;
using MouseButtonSimpleCallback = std::function<void()>;

public:
    /**
        Callback to notify about any mouse button event happening on the node.
        
        @param callback Callback to call which receives in the button, action, xPos, yPos
     */
    void setOnMouseButton(const MouseButtonCallback& callback);

    /**
        Callback to notify about mouse button left click happening on the node.
        
        @param callback Callback to call. Doesn't receive any params
     */
    void setOnMouseButtonLeftClick(const MouseButtonSimpleCallback& callback);

    /**
        Manually call the callback of @ref setOnMouseButton.
        
        @param btn Button that triggered the event
        @param action Action triggered (press/release/repeat)
        @param x X mouse position
        @param y Y mouse position
     */
    void callOnMouseButton(int32_t btn, int32_t action, int32_t x, int32_t y);

private:
    MouseButtonSimpleCallback mouseButtonSimpleCallback_{nullptr};
    MouseButtonCallback mouseButtonCallback_{nullptr};
};

class SliderListeners : public Listeners
{
using SlideValueChangedCallback = std::function<void(float)>;

public:
    /**
        Callback to notify about slider value change.

        @param callback Callback to call which receives in the current value of the slider
     */
    void setOnSlideValueChanged(const SlideValueChangedCallback& callback);

    /**
        Manually call the callback of @ref setOnSlideValueChanged.
        
        @param btn New value of the slider
     */
    void callOnSlide(float value);

private:
    SlideValueChangedCallback slideValueChangedCallback_{nullptr};
};
} // namespace msgui