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

/* Gateway to handle events coming from the OS window */
class Input
{
using MouseMoveCallback = std::function<void(const double x, const double y)>;
using MouseClickCallback = std::function<void(const int32_t btn, const int32_t action)>;
using MouseWheelCallback = std::function<void(const double offsetX, const double offsetY)>;
using MouseEnterExitWindowCallback = std::function<void(const bool entered)>;
using WindowResizeCallback = std::function<void(const uint32_t width, const uint32_t height)>;
using KeyCallback = std::function<void(const int32_t key, const int32_t scanCode, const int32_t mods)>;
using RefreshCallback = std::function<void()>;

public:
    Input(const Window* window);

    /**
        Set handler for when the mouse is moved.

        @param callback Function to be ran
    */
    void onMouseMove(const MouseMoveCallback& callback);

    /**
        Set handler for when any mouse button event occurs.

        @param callback Function to be ran
    */
    void onMouseButton(const MouseClickCallback& callback);

    /**
        Set handler for when a mouse wheel scroll event occurs.

        @param callback Function to be ran
    */
    void onMouseWheel(const MouseWheelCallback& callback);

    /**
        Set handler for when the mouse enters/exits the content area of the window.

        @param callback Function to be ran
    */
    void onMouseEnterExitWindow(const MouseEnterExitWindowCallback& callback);

    /**
        Set handler for when the window is resized.

        @param callback Function to be ran
    */
    void onWindowResize(const WindowResizeCallback& callback);

    /**
        Set handler for when a keyboard key is pressed.

        @param callback Function to be ran
    */
    void onKeyPress(const KeyCallback& callback);

    /**
        Set handler for when a keyboard key is released.

        @param callback Function to be ran
    */
    void onKeyRelease(const KeyCallback& callback);

    /**
        Set handler for when a keyboard key is held down.

        @param callback Function to be ran
    */
    void onKeyHold(const KeyCallback& callback);

    /**
        Set handler for when a keyboard key is pressed AND held down.

        @param callback Function to be ran
    */
    void onKeyPressAndHold(const KeyCallback& callback);

    /**
        Set handler for when the window is refreshed for any reason.

        @param callback Function to be ran
    */
    void onRefresh(const RefreshCallback& callback);

private:
    void setupEventCallbacks();

private:
    Logger log_;

    const Window* window_{nullptr};
    std::string windowName_{"UNKNOWN"};

    MouseMoveCallback mouseMoveCb_{nullptr};
    MouseClickCallback mouseClickCb_{nullptr};
    MouseWheelCallback mouseWheelCb_{nullptr};
    MouseEnterExitWindowCallback mouseEnterExitWindowCb_{nullptr};
    WindowResizeCallback winResizeCb_{nullptr};
    KeyCallback keyPressCb_{nullptr};
    KeyCallback keyReleaseCb_{nullptr};
    KeyCallback keyHoldCb_{nullptr};
    KeyCallback keyPressAndHoldCb_{nullptr};
    RefreshCallback refreshCb_{nullptr};
};
} // namespace msgui
