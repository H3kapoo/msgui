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
    void setOnMouseButton(const MouseButtonCallback& callback);
    void setOnMouseButtonLeftClick(const MouseButtonSimpleCallback& callback);

    void callOnMouseButton(int32_t btn, int32_t action, int32_t x, int32_t y);

private:
    MouseButtonCallback mouseButtonCallback_{nullptr};
    MouseButtonSimpleCallback mouseButtonSimpleCallback_{nullptr};
};

class SliderListeners : public Listeners
{
using SlideValueChangedCallback = std::function<void(float)>;

public:
    void setOnSlideValueChanged(const SlideValueChangedCallback& callback);

    void callOnSlide(float value);

private:
    SlideValueChangedCallback slideValueChangedCallback_{nullptr};
};
} // namespace msgui