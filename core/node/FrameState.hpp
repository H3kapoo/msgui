#pragma once

#include <memory>
#include <functional>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace msgui
{
#define MAKE_LAYOUT_DIRTY      if (getState()) { getState()->isLayoutDirty = true; };
#define REQUEST_STORE_RECREATE if (getState()) { getState()->layoutStoreNeedsRecreate = true; };
#define REQUEST_NEW_FRAME      if (getState()) { getState()->requestNewFrameFunc(); };
#define MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME\
    MAKE_LAYOUT_DIRTY \
    REQUEST_STORE_RECREATE\
    REQUEST_NEW_FRAME\

/* Cannot include AbstractNode.hpp due to imminent gl/glfw conflicts */
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
    int32_t lastMouseX                              {NO_VALUE};
    int32_t lastMouseY                              {NO_VALUE};
    glm::ivec2 frameSize                             {NO_VALUE, NO_VALUE};
    AbstractNodePtr clickedNodePtr                  {NO_PTR};
    AbstractNodePtr hoveredNodePtr                  {NO_PTR};
    std::function<void()> requestNewFrameFunc       {nullptr};
    bool isLayoutDirty                              {true};
    bool layoutStoreNeedsRecreate                   {true};
    int32_t currentCursorId                         {GLFW_ARROW_CURSOR};
    int32_t prevCursorId                            {GLFW_ARROW_CURSOR};
};

using FrameStatePtr = std::shared_ptr<FrameState>;
} // msgui