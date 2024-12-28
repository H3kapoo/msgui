#pragma once

#include <memory>

#include <GLFW/glfw3.h>

namespace msgui
{
struct FrameState
{
    int32_t mouseButtonState[GLFW_MOUSE_BUTTON_LAST];
    bool isLayoutDirty{true};
};
using FrameStatePtr = std::shared_ptr<FrameState>;
} // msgui