#include "WindowFrame.hpp"
#include "msgui/events/MouseEnter.hpp"
#include "msgui/events/MouseExit.hpp"
#include "msgui/events/WindowResize.hpp"
#include "msgui/layoutEngine/CustomLayoutEngine.hpp"

#include <algorithm>
#include <memory>
#include <queue>
#include <ranges>

#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "msgui/layoutEngine/BasicLayoutEngine.hpp"
#include "msgui/layoutEngine/BasicTextLayoutEngine.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/events/FocusLost.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/events/WheelScroll.hpp"
#include "msgui/renderer/NodeRenderer.hpp"
#include "msgui/renderer/TextBufferStore.hpp"
#include "msgui/vendor/stb_image_write.h"

namespace msgui
{
std::array<GLFWcursor*, WindowFrame::MAX_DEFAULT_CURSORS> WindowFrame::standardCursors_ = {0};
bool WindowFrame::initCursors = true;

WindowFrame::WindowFrame(const std::string& windowName, const uint32_t width, const uint32_t height, const bool isPrimary)
    : log_("WindowFrame(" + windowName + ")")
    , window_(windowName, width, height)
    , input_(&window_)
    , frameState_(std::make_shared<FrameState>())
    // , layoutEngine_(std::make_shared<BasicLayoutEngine>())
    , layoutEngine_(std::make_shared<CustomLayoutEngine>())
    , textLayoutEngine_(std::make_shared<BasicTextLayoutEngine>())
    , frameBox_(std::make_shared<Box>(windowName))
    , isPrimary_(isPrimary)
{
    /* Setup GLFW input events */
    input_.onWindowResize([this](uint32_t newWidth, uint32_t newHeight)
    {
        frameBox_->transform_.scale = {newWidth, newHeight, 1};
        frameBox_->transform_.vScale = {newWidth, newHeight};
        window_.setTitle(std::to_string(newWidth) + " " + std::to_string(newHeight));
        window_.onResizeEvent(newWidth, newHeight);
        resolveOnWindowReizeFromInput(newWidth, newHeight);
    });

    input_.onKeyPress([this](int32_t key, int32_t scanCode, int32_t mods)
    {
        (void)scanCode;
        (void)mods;

        if (key == GLFW_KEY_Q)
        {
            log_.infoLn("Quit called");
            shouldWindowClose_ = true;
        }
    });

    input_.onMouseButton(
        std::bind(
            &WindowFrame::resolveOnMouseButtonFromInput,
            this, std::placeholders::_1, std::placeholders::_2));
    
    input_.onMouseMove(
        std::bind(
            &WindowFrame::resolveOnMouseMoveFromInput,
            this, std::placeholders::_1, std::placeholders::_2));

    input_.onMouseWheel(
        std::bind(
            &WindowFrame::resolveOnMouseWheelFromInput,
            this, std::placeholders::_1, std::placeholders::_2));

    input_.onMouseEnterExitWindow(
        std::bind(
            &WindowFrame::resolveOnMouseEnterExitFromInput,
            this, std::placeholders::_1));

    /* Setup internal objects */
    frameState_->requestNewFrameFunc = Window::requestEmptyEvent;
    frameState_->frameSize = {width, height};

    frameBox_->setColor(Utils::hexToVec4("#cc338bff"));
    frameBox_->transform_.pos = {0, 0, 1};
    frameBox_->transform_.vPos = {frameBox_->getLayout().border.left, frameBox_->getLayout().border.top};
    frameBox_->transform_.scale = {width, height, 1};
    frameBox_->transform_.vScale = {
        width - frameBox_->getLayout().border.left - frameBox_->getLayout().border.left,
        height - frameBox_->getLayout().border.top - frameBox_->getLayout().border.bot};
    frameBox_->state_ = frameState_;

    /* Init cursors */
    if (initCursors)
    {
        standardCursors_ =
        {
            glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
            glfwCreateStandardCursor(GLFW_IBEAM_CURSOR),
            glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR),
            glfwCreateStandardCursor(GLFW_HAND_CURSOR),
            glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR),
            glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR)
        };
        log_.infoLn("Standard cursors initialized!");
        initCursors = false;
    }
}

WindowFrame::~WindowFrame()
{
    log_.infoLn("Cleaning up frameState..");
    frameState_->clickedNodePtr = NO_PTR;
    frameState_->prevClickedNodePtr = NO_PTR;
    frameState_->hoveredNodePtr = NO_PTR;

    if (!initCursors)
    {
        for (GLFWcursor* cursor : standardCursors_)
        {
            glfwDestroyCursor(cursor);
        }
        initCursors = true;
    }
}

void WindowFrame::saveBufferToFile(const std::string& filePath, const int32_t quality) const
{
    if (filePath.empty())
    {
        log_.warnLn("Empty file received for screenshot path!");
        return;
    }

    const auto& w = window_.getWidth();
    const auto& h = window_.getHeight();
    const int32_t comps = 3;

    std::unique_ptr<uint8_t[]> store(new uint8_t[w * h * comps]);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, store.get());
    stbi_flip_vertically_on_write(true);

    int32_t res = stbi_write_jpg(filePath.c_str(), w, h, comps, store.get(), quality);
    if (res != 0)
    {
        log_.infoLn("Screenshot written to: %s", filePath.c_str());
    }
    else
    {
        log_.errorLn("Failed to get and write screenshot to: %s", filePath.c_str());
    }
}

BoxPtr WindowFrame::getRoot()
{
    return frameBox_;
}

bool WindowFrame::isPrimary() const
{
    return isPrimary_;
}

bool WindowFrame::run()
{
    /* See if cursor needs changing */
    if (frameState_->currentCursorId != frameState_->prevCursorId)
    {
        frameState_->prevCursorId = frameState_->currentCursorId;
        int32_t idx = frameState_->currentCursorId - GLFW_ARROW_CURSOR;
        glfwSetCursor(window_.getHandle(), standardCursors_[idx]);
    }

    /* Layout pass */
    if (frameState_->layoutPassActions != ELayoutPass::NOTHING)
    {
        updateLayout();

        /* If the layout got dirty again we need to simulate a new frame RUN request. */
        if (frameState_->layoutPassActions != ELayoutPass::NOTHING)
        {
            Window::requestEmptyEvent();
            return false; // do not render yet, go away.
        }
    }

    /* Render pass */
    window_.setContextCurrent();
    window_.setCurrentViewport();
    window_.setCurrentScissorArea();
    Window::clearColor(glm::vec4{0.0, 1.0, 0.0, 1.0f});
    Window::clearBits(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderLayout();
    window_.swap();

    return shouldWindowClose_ || window_.shouldClose();
}

void WindowFrame::renderLayout()
{
    const auto pMat = window_.getProjectionMat();

    /* Currently we render front to back to minimize overdraw by making use of the depth buffer.
       Not sure how this will work when we need alpha blending between nodes. Maybe we will need
       to render back to front in that case and take overdrawing as a compromise. */
    /* TODO: Deal with transparent objects. Current fix is to render back to front (reverse) when there are
       transparent objects. */
    // Later Note: for UI apps it may really not matter performance wise to render front to back.
    for (auto& node : allFrameChildNodes_ | std::views::reverse) // -> back to front Z
    // for (auto& node : allFrameChildNodes_) // -> front to back Z
    {
        renderer::NodeRenderer::render(node, pMat, frameState_->frameSize.y);
    }

    /* Render text after the nodes themselves. */
    textRenderer_.render(pMat, frameState_->frameSize.y);
}

void WindowFrame::updateLayout()
{
    /* Must redo internal vector structure if something was added/removed. */
    if (frameState_->layoutPassActions & ELayoutPass::RESOLVE_NODE_RELATIONS)
    {
        frameState_->layoutPassActions &= ~ELayoutPass::RESOLVE_NODE_RELATIONS;
        resolveNodeRelations();
    }

    /* Iterate from lowest depth to highest */
    const bool isNodeTrRecalc = frameState_->layoutPassActions & ELayoutPass::RECALCULATE_NODE_TRANSFORM;
    if (isNodeTrRecalc)
    {
        /* It's important for the variable to be reset before the layout update is called as layoutUpdate
            can SET the variable to true again if it decides the layout got dirty. */
        frameState_->layoutPassActions &= ~ELayoutPass::RECALCULATE_NODE_TRANSFORM;

        for (const auto& node : allFrameChildNodes_ | std::views::reverse)
        {
            CustomLayoutEngine::Result<glm::vec2> result = layoutEngine_->process(node);
            if (!result.error.empty())
            {
                log_.errorLn("Error in layout calc while processing '%s': %s", node->getCName(), result.error.c_str());
                return;
            }

            /* Currently only BOX type nodes support overflow handling */
            // if (node->getType() == AbstractNode::NodeType::BOX)
            // {
            //     Utils::as<Box>(node)->updateOverflow(result.value);
            // }
    
            /* TODO: This shall be moved into layout process(). */
            /* After updating the node layout, we need to update the viewable area of the node based on the parent's
               viewable area. Raw parent is used for better performance (compared to locking each time). */
            if (auto parent = node->getParentRaw())
            {
                /* Dropdown's box child needs to ignore using the BB of the parent to compute viewable area. Use
                   the area of the window itself instead. Also FloatingBox shall be unafected by viewarea. */
                if (parent->getType() == AbstractNode::NodeType::DROPDOWN
                || node->getType() == AbstractNode::NodeType::FLOATING_BOX
            )
                // if (node->getType() == AbstractNode::NodeType::FLOATING_BOX)
                {
                    auto frameBoxIdx = allFrameChildNodes_.size() - 1;
                    node->transform_.computeViewableArea(allFrameChildNodes_[frameBoxIdx]->transform_, utils::Layout::TBLR{0});
                }
                /* Otherwise just compute viewable area as normal. */
                else
                {
                    node->transform_.computeViewableArea(parent->transform_, parent->getLayout().border);
                }
            }
        }
    }

    /* Update text layouts if needed. */
    auto& textBuffer = renderer::TextBufferStore::get().buffer();
    for (auto& textData : textBuffer)
    {
        textLayoutEngine_->process(textData, isNodeTrRecalc);
    }
}

void WindowFrame::resolveNodeRelations()
{
    allFrameChildNodes_.clear();

    std::queue<AbstractNodePtr> q;
    q.push(frameBox_);

    while (!q.empty())
    {
        AbstractNodePtr node = q.front();
        q.pop();

        allFrameChildNodes_.push_back(node);

        for (auto& ch : node->getChildren())
        {
            /* Set children's frameState and depth if needed */
            if (!ch->state_)
            {
                bool isScrollNode = ch->getType() == AbstractNode::NodeType::SCROLL;
                bool isDropdownNodeBox = ch->getType() == AbstractNode::NodeType::DROPDOWN;
                bool isFloatingBoxNode = ch->getType() == AbstractNode::NodeType::FLOATING_BOX;
                ch->parent_ = node;
                ch->parentRaw_ = node.get();
                ch->transform_.pos.z = node->transform_.pos.z + 1;
                
                if (isScrollNode)
                {
                    // needs to start from the down and go down progressively
                    ch->transform_.pos.z = SCROLL_LAYER_START - node->transform_.pos.z;
                }
                else if (isDropdownNodeBox)
                {
                    ch->transform_.pos.z += DROPDOWN_LAYER_START;
                }
                else if (isFloatingBoxNode)
                {
                    ch->transform_.pos.z += FLOATING_LAYER_START;
                }
                ch->state_ = frameState_;
            }

            q.push(ch);
        }
    }

    /* Sort nodes from high to low depth */
    std::ranges::sort(allFrameChildNodes_,
        [](const AbstractNodePtr a, const AbstractNodePtr b)
        {
            return a->getTransform().pos.z > b->getTransform().pos.z;
        });
}

void WindowFrame::resolveOnMouseButtonFromInput(const int32_t btn, const int32_t action)
{
    frameState_->mouseButtonState[btn] = action;
    frameState_->lastMouseButtonTriggeredIdx = btn;

    int32_t mX{frameState_->mouseX};
    int32_t mY{frameState_->mouseY};
    bool foundNode{false};
    for (const auto& node : allFrameChildNodes_)
    {
        /* Skip nodes marked as transparent. Events will be bubbled down to the next valid node. */
        if (node->isEventTransparent()) { continue; }

        glm::ivec2& nodePos = node->transform_.vPos;
        glm::ivec2& nodeScale = node->transform_.vScale;
        if ((mX >= nodePos.x && mX <= nodePos.x + nodeScale.x) &&
            (mY >= nodePos.y && mY <= nodePos.y + nodeScale.y))
        {
            foundNode = true;
            if (frameState_->mouseButtonState[btn])
            {
                frameState_->clickedNodePtr = node;
                auto prevNode = frameState_->prevClickedNodePtr.lock();
                auto hoveredNode = frameState_->hoveredNodePtr.lock();
                if (prevNode && prevNode != node)
                {
                    events::FocusLost evt;
                    prevNode->getEvents().notifyAllChannels<events::FocusLost>(evt);
                }
                
                if (btn == GLFW_MOUSE_BUTTON_LEFT)
                {
                    events::LMBClick evt;
                    node->getEvents().notifyAllChannels<events::LMBClick>(evt);
                }
                else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    // events::RMBClick evt;
                    // node->getEvents().notifyAllChannels<events::RMBClick>(evt);
                }
            }
            else if (!frameState_->mouseButtonState[btn])
            {
                frameState_->prevClickedNodePtr = frameState_->clickedNodePtr;
                if (frameState_->prevClickedNodePtr.lock() && node != frameState_->prevClickedNodePtr.lock())
                {
                    if (btn == GLFW_MOUSE_BUTTON_LEFT)
                    {
                        events::LMBReleaseNotHovered evt;
                        frameState_->prevClickedNodePtr.lock()->getEvents()
                        .notifyAllChannels<events::LMBReleaseNotHovered>(evt);
                    }
                    else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                    {
                        // TODO: To be fille if neeeded
                    }
                }

                if (btn == GLFW_MOUSE_BUTTON_LEFT)
                {
                    if (frameState_->clickedNodePtr.lock() == frameState_->prevClickedNodePtr.lock())
                    {
                        events::LMBRelease evt{{mX, mY}};
                        node->getEvents().notifyAllChannels<events::LMBRelease>(evt);
                    }
                }
                else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    events::RMBRelease evt{{mX, mY}};
                    node->getEvents().notifyAllChannels<events::RMBRelease>(evt);
                }

                frameState_->clickedNodePtr = NO_PTR;
            }
            break; // event was consumed
        }
    }

    /* Means we released the button somewhere outside of the window. */
    if (!foundNode)
    {
        frameState_->prevClickedNodePtr = frameState_->clickedNodePtr;
        events::LMBReleaseNotHovered evt;
        frameState_->prevClickedNodePtr.lock()->getEvents().notifyAllChannels<events::LMBReleaseNotHovered>(evt);
        frameState_->clickedNodePtr = NO_PTR;
    }
}

void WindowFrame::resolveOnMouseMoveFromInput(const int32_t x, const int32_t y)
{
    frameState_->lastMouseX = frameState_->mouseX;
    frameState_->lastMouseY = frameState_->mouseY;
    frameState_->mouseX = x;
    frameState_->mouseY = y;

    for (const auto& node : allFrameChildNodes_)
    {
        /* Skip nodes marked as transparent. Events will be bubbled down to the next valid node. */
        if (node->isEventTransparent()) { continue; }

        glm::ivec2& nodePos = node->transform_.vPos;
        glm::ivec2& nodeScale = node->transform_.vScale;
        if ((x >= nodePos.x && x <= nodePos.x + nodeScale.x) &&
            (y >= nodePos.y && y <= nodePos.y + nodeScale.y))
        {
            if (node != frameState_->hoveredNodePtr.lock())
            {
                events::MouseEnter evtEnter;
                node->getEvents().notifyAllChannels(evtEnter);
                if (auto prevHoveredNode = frameState_->hoveredNodePtr.lock())
                {
                    events::MouseExit evtExit;
                    prevHoveredNode->getEvents().notifyAllChannels(evtExit);
                }

                frameState_->hoveredNodePtr = node;
            }

            break; // event was consumed
        }
    }

    /* Following will try to find the nearest scrollbar from the hovered node's position.
       This loop will run at most MAX_HEIGHT_OF_UI_TREE times in the worst case. */
    frameState_->nearScrollNodePtr = NO_PTR;
    AbstractNodePtr p = frameState_->hoveredNodePtr.lock();
    while (p != NO_PTR)
    {
        if (p->getType() == AbstractNode::NodeType::SLIDER_KNOB)
        {
            frameState_->nearScrollNodePtr = p->getParent();
        }
        else if (p->getType() == AbstractNode::NodeType::SLIDER)
        {
            frameState_->nearScrollNodePtr = p;
        }
        else if (p->getType() == AbstractNode::NodeType::BOX
            || p->getType() == AbstractNode::NodeType::TREEVIEW) // TODO: || NodeType is TreeView/RecycleList
        {
            auto box = Utils::as<Box>(p);
            if (box->isScrollBarActive(utils::Layout::Type::VERTICAL))
            {
                frameState_->nearScrollNodePtr = box->getVBar();
            }
            // TODO: When CTRL is held pick the horizontal direction instead of the verical one
            else if (box->isScrollBarActive(utils::Layout::Type::HORIZONTAL))
            {
                frameState_->nearScrollNodePtr = box->getHBar();
            }
        }

        if (frameState_->nearScrollNodePtr.lock())
        {
            break;
        }
        p = p->getParent().lock();
    }

    /* Having a selectedNodeId && currently holding down left click means we want to drag only. */
    if (frameState_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT] && frameState_->clickedNodePtr.lock() != NO_PTR)
    {
        events::LMBDrag evt(x, y);
        frameState_->clickedNodePtr.lock()->getEvents().notifyAllChannels<events::LMBDrag>(evt);
        return;
    }
}

void WindowFrame::resolveOnMouseWheelFromInput(const int32_t x, const int32_t y)
{
    /* Note: Yes. GLFW will return to us "double" for this input event and not int32 BUT
        at least on Linux, the return values are -1, 0, 1 and so we can just treat them as ints.
    */
    if (auto node = frameState_->nearScrollNodePtr.lock())
    {
        events::WheelScroll evt{y};
        node->getEvents().notifyAllChannels(evt);
    }
}

void WindowFrame::resolveOnMouseEnterExitFromInput(const bool entered)
{
    if (auto hoveredNode = frameState_->hoveredNodePtr.lock(); hoveredNode && !entered)
    {
        events::MouseExit evtExit;
        hoveredNode->getEvents().notifyAllChannels(evtExit);
        frameState_->hoveredNodePtr = NO_PTR;
    }
}

void WindowFrame::resolveOnWindowReizeFromInput(const int32_t newWidth, const int32_t newHeight)
{
    frameState_->layoutPassActions = ELayoutPass::EVERYTHING_NODE;
    frameState_->frameSize = {newWidth, newHeight};

    events::WindowResize evt;
    for (const auto& node : allFrameChildNodes_)
    {
        node->getEvents().notifyAllChannels(evt);
    }
}
} // namespace msgui