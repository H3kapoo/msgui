#include "Input.hpp"

namespace msgui
{

Input::Input(const Window* window)
    : log_("Input(" + window->getName() + ")")
    , window_(window)
{
    setupEventCallbacks();
    log_.infoLn("Callback slots have been initialized");
}

void Input::onMouseMove(const MouseMoveCallback& callback) { mouseMoveCb_ = callback; }

void Input::onMouseButton(const MouseClickCallback& callback) { mouseClickCb_ = callback; }

void Input::onMouseWheel(const MouseWheelCallback& callback) { mouseWheelCb_ = callback; }

void Input::onWindowResize(const WindowResizeCallback& callback) { winResizeCb_ = callback; }

void Input::onKeyPress(const KeyCallback& callback) { keyPressCb_ = callback; }

void Input::onKeyRelease(const KeyCallback& callback) { keyReleaseCb_ = callback; }

void Input::onKeyHold(const KeyCallback& callback) { keyHoldCb_ = callback; }

void Input::onKeyPressAndHold(const KeyCallback& callback) { keyPressAndHoldCb_ = callback; }

void Input::onRefresh(const RefreshCallback& callback) { refreshCb_ = callback; }

void Input::setupEventCallbacks()
{
    GLFWwindow* windowHandle = window_->getHandle();
    glfwSetWindowUserPointer(windowHandle, this);

    glfwSetWindowRefreshCallback(windowHandle,
        [](GLFWwindow* win)
        {
            Input* input =  static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->refreshCb_)
            {
                input->refreshCb_();
            };
        });

    glfwSetCursorPosCallback(windowHandle,
        [](GLFWwindow* win, double xPos, double yPos)
        {
            Input* input =  static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->mouseMoveCb_)
            {
                input->mouseMoveCb_(xPos, yPos);
            };
        });

    glfwSetMouseButtonCallback(windowHandle,
        [](GLFWwindow* win, int32_t button, int32_t action, int32_t mods)
        {
            (void)button;
            (void)mods;

            Input* input =  static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->mouseClickCb_)
            {
                input->mouseClickCb_(button, action);
            };
        });

    glfwSetFramebufferSizeCallback(windowHandle,
        [](GLFWwindow* win, int32_t width, int32_t height)
        {
            Input* input =  static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->winResizeCb_)
            {
                input->winResizeCb_(width, height);
            }
        });

    glfwSetKeyCallback(windowHandle,
        [](GLFWwindow* win, int32_t key, int32_t scancode, int32_t action, int32_t mods)
        {
            Input* input = static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->keyPressCb_ && action == GLFW_PRESS)
            {
                input->keyPressCb_(key, scancode, mods);
            }
            else if (input->keyReleaseCb_ && action == GLFW_RELEASE)
            {
                input->keyReleaseCb_(key, scancode, mods);
            }
            else if (input->keyHoldCb_ && action == GLFW_REPEAT)
            {
                input->keyHoldCb_(key, scancode, mods);
            }

            if (input->keyPressAndHoldCb_ && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                input->keyPressAndHoldCb_(key, scancode, mods);
            }
        });

    glfwSetScrollCallback(windowHandle,
        [](GLFWwindow* win, double offsetX, double offsetY)
        {
            Input* input = static_cast<Input*>(glfwGetWindowUserPointer(win));
            if (input->mouseWheelCb_)
            {
                input->mouseWheelCb_(offsetX, offsetY);
            }
        });
}
} // namespace msgui
