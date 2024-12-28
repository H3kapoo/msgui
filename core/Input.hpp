#pragma once

#include <cstdint>
#include <string>
#include <functional>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Logger.hpp"

namespace msgui
{
using MouseMoveCallback = std::function<void(double x, double y)>;
using MouseClickCallback = std::function<void(int32_t btn, int32_t action)>;
using WindowResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
using KeyCallback = std::function<void(int32_t key, int32_t scanCode, int32_t mods)>;
using RefreshCallback = std::function<void()>;

class Input
{
private:
    void setupEventCallbacks();

public:
    Input(const Window* window);

    void onMouseMove(const MouseMoveCallback& callback);
    void onMouseButton(const MouseClickCallback& callback);
    void onWindowResize(const WindowResizeCallback& callback);
    void onKeyPress(const KeyCallback& callback);
    void onKeyRelease(const KeyCallback& callback);
    void onKeyHold(const KeyCallback& callback);
    void onKeyPressAndHold(const KeyCallback& callback);
    void onRefresh(const RefreshCallback& callback);

private:
    Logger log_;

    const Window* window_{nullptr};
    std::string windowName_{"UNKNOWN"};

    MouseMoveCallback mouseMoveCb_{nullptr};
    MouseClickCallback mouseClickCb_{nullptr};
    WindowResizeCallback winResizeCb_{nullptr};
    KeyCallback keyPressCb_{nullptr};
    KeyCallback keyReleaseCb_{nullptr};
    KeyCallback keyHoldCb_{nullptr};
    KeyCallback keyPressAndHoldCb_{nullptr};
    RefreshCallback refreshCb_{nullptr};
};
} // namespace msgui
