#pragma once

#include <memory>
#include <functional>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace msgui
{
// TODO: Implement bitwise & and | so we dont rely on declaring it as uint8_t in FrameState
enum ELayoutPass : uint8_t
{
    NOTHING                    = 0b00000000,
    RECALCULATE_NODE_TRANSFORM = 0b00000001,
    RECALCULATE_TEXT_TRANSFORM = 0b00000010,
    RESOLVE_NODE_RELATIONS     = 0b00000100,
    EVERYTHING_NODE            = RECALCULATE_NODE_TRANSFORM | RESOLVE_NODE_RELATIONS,
    EVERYTHING_TEXT            = RECALCULATE_TEXT_TRANSFORM,
    EVERYTHING                 = EVERYTHING_NODE | EVERYTHING_TEXT
};

#define MAKE_TEXT_LAYOUT_DIRTY if (getState()) { getState()->layoutPassActions |= ELayoutPass::EVERYTHING_TEXT;  };
#define MAKE_LAYOUT_DIRTY      if (getState()) { getState()->layoutPassActions |= ELayoutPass::RECALCULATE_NODE_TRANSFORM; };
#define REQUEST_STORE_RECREATE if (getState()) { getState()->layoutPassActions |= ELayoutPass::RESOLVE_NODE_RELATIONS; };
#define REQUEST_NEW_FRAME      if (getState()) { getState()->requestNewFrameFunc(); };
#define MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME\
    MAKE_LAYOUT_DIRTY \
    REQUEST_STORE_RECREATE\
    REQUEST_NEW_FRAME\

/* Cannot include AbstractNode.hpp due to imminent gl/glfw conflicts */
class AbstractNode;
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
using AbstractNodeWPtr = std::weak_ptr<AbstractNode>;

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
    glm::ivec2 frameSize                            {NO_VALUE, NO_VALUE};
    AbstractNodeWPtr clickedNodePtr                 {NO_PTR};
    AbstractNodeWPtr prevClickedNodePtr             {NO_PTR};
    AbstractNodeWPtr hoveredNodePtr                  {NO_PTR};
    std::function<void()> requestNewFrameFunc       {nullptr};
    uint8_t layoutPassActions                       {ELayoutPass::EVERYTHING_NODE};
    int32_t currentCursorId                         {GLFW_ARROW_CURSOR};
    int32_t prevCursorId                            {GLFW_ARROW_CURSOR};
};

using FrameStatePtr = std::shared_ptr<FrameState>;
} // msgui