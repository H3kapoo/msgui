#pragma once

#include <memory>

#include <GLFW/glfw3.h>

namespace msgui
{
// Cannot include AbstractNode.hpp due to imminent gl/glfw conflicts
class AbstractNode;
using AbstractNodePtr = std::shared_ptr<AbstractNode>;

static constexpr int32_t NO_ID = 0;
static constexpr int32_t NO_VALUE = 0;
static const AbstractNodePtr NO_PTR = nullptr;

struct FrameState
{
    int32_t mouseButtonState[GLFW_MOUSE_BUTTON_LAST]{NO_VALUE};
    int32_t lastMouseButtonTriggeredIdx             {NO_VALUE};
    int32_t mouseX                                  {NO_VALUE};
    int32_t mouseY                                  {NO_VALUE};
    AbstractNodePtr clickedNodePtr                  {NO_PTR};
    bool isLayoutDirty                              {true};
    bool layoutNeedsSort                            {true};
    bool layoutStoreNeedsRecreate                   {true};
};
using FrameStatePtr = std::shared_ptr<FrameState>;
} // msgui