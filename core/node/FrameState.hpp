#pragma once

#include <memory>

#include <GLFW/glfw3.h>

namespace msgui
{
struct FrameState
{
    int32_t mouseButtonState[GLFW_MOUSE_BUTTON_LAST];
    int32_t lastMouseButtonTriggeredIdx{0};
    int32_t mouseX{0};
    int32_t mouseY{0};
    bool isLayoutDirty{true};
};
using FrameStatePtr = std::shared_ptr<FrameState>;
} // msgui