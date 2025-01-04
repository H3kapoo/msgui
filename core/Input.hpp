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

class Input
{
// Callback Defs
using MouseMoveCallback = std::function<void(const double x, const double y)>;
using MouseClickCallback = std::function<void(const int32_t btn, const int32_t action)>;
using WindowResizeCallback = std::function<void(const uint32_t width, const uint32_t height)>;
using KeyCallback = std::function<void(const int32_t key, const int32_t scanCode, const int32_t mods)>;
using RefreshCallback = std::function<void()>;

public:
    Input(const Window* window);

    // Event receiver callbacks
    void onMouseMove(const MouseMoveCallback& callback);
    void onMouseButton(const MouseClickCallback& callback);
    void onWindowResize(const WindowResizeCallback& callback);
    void onKeyPress(const KeyCallback& callback);
    void onKeyRelease(const KeyCallback& callback);
    void onKeyHold(const KeyCallback& callback);
    void onKeyPressAndHold(const KeyCallback& callback);
    void onRefresh(const RefreshCallback& callback);

private:
    // Normal
    void setupEventCallbacks();

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
