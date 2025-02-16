#include "Listeners.hpp"

#include <GLFW/glfw3.h>

namespace msgui
{
void Listeners::setOnMouseButton(const MouseButtonCallback& callback)
{
    mouseButtonCallback_ = callback;
}

void Listeners::setOnMouseButtonLeftClick(const MouseButtonSimpleCallback& callback)
{
    mouseButtonSimpleCallback_ = callback;
}

void Listeners::callOnMouseButton(int32_t btn, int32_t action, int32_t x, int32_t y)
{
    if (mouseButtonCallback_)
    {
        mouseButtonCallback_(btn, action, x, y);
    }

    if (mouseButtonSimpleCallback_ && btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouseButtonSimpleCallback_();
    }
}

void SliderListeners::setOnSlideValueChanged(const SlideValueChangedCallback& callback)
{
    slideValueChangedCallback_ = callback;
}

void SliderListeners::callOnSlide(float value)
{
    if (slideValueChangedCallback_)
    {
        slideValueChangedCallback_(value);
    }
}
} // namespace msgui