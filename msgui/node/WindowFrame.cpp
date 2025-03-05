#include "WindowFrame.hpp"
#include "msgui/node/RecycleList.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBDrag.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/LMBReleaseNotHovered.hpp"
#include "msgui/nodeEvent/NodeEventManager.hpp"
#include "msgui/nodeEvent/RMBRelease.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <ranges>
#include <queue>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "msgui/vendor/stb_image_write.h"

#include "msgui/layoutEngine/SimpleLayoutEngine.hpp"
#include "msgui/Renderer.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/FrameState.hpp"

namespace msgui
{
std::array<GLFWcursor*, 6> WindowFrame::standardCursors_ = {0};
bool WindowFrame::initCursors = true;

WindowFrame::WindowFrame(const std::string& windowName, const uint32_t width, const uint32_t height, const bool isPrimary)
    : log_("WindowFrame(" + windowName + ")")
    , window_(windowName, width, height)
    , input_(&window_)
    , frameState_(std::make_shared<FrameState>())
    , layoutEngine_(std::make_shared<SimpleLayoutEngine>())
    , frameBox_(std::make_shared<Box>(windowName))
    , isPrimary_(isPrimary)
{
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

    frameState_->requestNewFrameFunc = Window::requestEmptyEvent;
    frameState_->frameSize = {width, height};

    frameBox_->setColor(Utils::hexToVec4("#cc338bff"));
    frameBox_->transform_.pos = {0, 0, 1};
    frameBox_->transform_.scale = {width, height, 1};
    frameBox_->transform_.vPos = {frameBox_->getLayout().border.left, frameBox_->getLayout().border.top};
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
    if (frameState_->isLayoutDirty)
    {
        /* It's important for the variable to be reset before the layout update is called as layoutUpdate
           can SET the variable to true again if it decides the layout got dirty. */
        frameState_->isLayoutDirty = false;
        updateLayout();

        /* If the layout got dirty again we need to simulate a new frame RUN request. */
        if (frameState_->isLayoutDirty)
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
    // for (auto& node : allFrameChildNodes_ | std::views::reverse) // -> back to front Z
    for (auto& node : allFrameChildNodes_) // -> front to back Z
    {
        Renderer::render(node, pMat, frameState_->frameSize.y);
    }
}

void WindowFrame::updateLayout()
{
    /* Must redo internal vector structure if something was added/removed. */
    if (frameState_->layoutStoreNeedsRecreate)
    {
        resolveNodeRelations();
    }

    /* Iterate from lowest depth to highest */
    for (auto& node : allFrameChildNodes_ | std::views::reverse)
    {
        glm::ivec2 overflow = layoutEngine_->process(node);

        /* Currently only BOX type nodes support overflow handling */
        if (node->getType() == AbstractNode::NodeType::BOX)
        {
            static_cast<Box*>(node.get())->updateOverflow(overflow);
        }

        /* TODO: This shall be moved into layout process(). */
        /* After updating the node layout, we need to update the viewable area of the node based on the parent's
           viewable area. Raw parent is used for better performance (compared to locking each time). */
        if (auto p = node->getParentRaw())
        {
            /* RecyleList and also TreeView, being the king it is, requires that we tell it that the layout pass
               for it had finished. Note that we do this on the parent of the current BOX node because we need to
               notify it AFTER the item containing BOX inside it finished the layout pass. */
            if (p->getType() == AbstractNode::NodeType::RECYCLE_LIST &&
                node->getType() == AbstractNode::NodeType::BOX)
            {
                static_cast<RecycleList*>(p)->onLayoutUpdateNotify();
            }
            else if (p->getType() == AbstractNode::NodeType::TREEVIEW &&
                node->getType() == AbstractNode::NodeType::BOX)
            {
                static_cast<TreeView*>(p)->onLayoutUpdateNotify();
            }

            /* Dropdown's box child needs to ignore using the BB of the parent to compute viewable area. Use
               the area of the window itself instead. */
            if (p->getType() == AbstractNode::NodeType::DROPDOWN)
            {
                auto frameBoxIdx = allFrameChildNodes_.size() - 1;
                node->transform_.computeViewableArea(allFrameChildNodes_[frameBoxIdx]->transform_, Layout::TBLR{0});
            }
            /* Otherwise just compute viewable area as normal. */
            else
            {
                node->transform_.computeViewableArea(p->transform_, p->getLayout().border);
            }
        }
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
                ch->transform_.pos.z = node->transform_.pos.z + (isScrollNode ? SCROLL_LAYER_START : 1);
                ch->transform_.pos.z += isDropdownNodeBox ? DROPDOWN_LAYER_START : 0;
                ch->transform_.pos.z = isFloatingBoxNode ? FLOATING_LAYER_START : ch->transform_.pos.z;
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
        glm::ivec2& nodePos = node->transform_.vPos;
        glm::ivec2& nodeScale = node->transform_.vScale;
        if ((mX >= nodePos.x && mX <= nodePos.x + nodeScale.x) &&
            (mY >= nodePos.y && mY <= nodePos.y + nodeScale.y))
        {
            foundNode = true;
            if (frameState_->mouseButtonState[btn])
            {
                frameState_->clickedNodePtr = node;
                auto& prevNode = frameState_->prevClickedNodePtr;
                auto& hoveredNode = frameState_->hoveredNodePtr;
                if (prevNode && prevNode != node)
                {
                    nodeevent::FocusLost evt;
                    prevNode->getEvents().notifyAllChannels<nodeevent::FocusLost>(evt);
                }
                
                if (btn == GLFW_MOUSE_BUTTON_LEFT)
                {
                    nodeevent::LMBClick evt;
                    node->getEvents().notifyAllChannels<nodeevent::LMBClick>(evt);
                }
                else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    // nodeevent::RMBClick evt;
                    // node->getEvents().notifyAllChannels<nodeevent::RMBClick>(evt);
                }
            }
            else if (!frameState_->mouseButtonState[btn])
            {
                frameState_->prevClickedNodePtr = frameState_->clickedNodePtr;
                if (frameState_->prevClickedNodePtr && node != frameState_->prevClickedNodePtr)
                {
                    if (btn == GLFW_MOUSE_BUTTON_LEFT)
                    {
                        nodeevent::LMBReleaseNotHovered evt;
                        frameState_->prevClickedNodePtr->getEvents()
                            .notifyAllChannels<nodeevent::LMBReleaseNotHovered>(evt);
                    }
                    else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                    {
                        // TODO: To be fille if neeeded
                    }
                }

                frameState_->clickedNodePtr = NO_PTR;
                
                if (btn == GLFW_MOUSE_BUTTON_LEFT)
                {
                    nodeevent::LMBRelease evt{{mX, mY}};
                    node->getEvents().notifyAllChannels<nodeevent::LMBRelease>(evt);
                }
                else if (btn == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    nodeevent::RMBRelease evt{{mX, mY}};
                    node->getEvents().notifyAllChannels<nodeevent::RMBRelease>(evt);
                }
            }
            break; // event was consumed
        }
    }

    /* Means we released the button somewhere outside of the window. */
    if (!foundNode)
    {
        frameState_->prevClickedNodePtr = frameState_->clickedNodePtr;
        nodeevent::LMBReleaseNotHovered evt;
        frameState_->prevClickedNodePtr->getEvents().notifyAllChannels<nodeevent::LMBReleaseNotHovered>(evt);
        frameState_->clickedNodePtr = NO_PTR;
    }
}

void WindowFrame::resolveOnMouseMoveFromInput(const int32_t x, const int32_t y)
{
    frameState_->lastMouseX = frameState_->mouseX;
    frameState_->lastMouseY = frameState_->mouseY;
    frameState_->mouseX = x;
    frameState_->mouseY = y;

    frameState_->hoveredNodePtr = NO_PTR;
    for (const auto& node : allFrameChildNodes_)
    {
        glm::ivec2& nodePos = node->transform_.vPos;
        glm::ivec2& nodeScale = node->transform_.vScale;
        if ((x >= nodePos.x && x <= nodePos.x + nodeScale.x) &&
            (y >= nodePos.y && y <= nodePos.y + nodeScale.y))
        {
            frameState_->hoveredNodePtr = node;
            // node->onMouseHoverNotify();
            break; // event was consumed
        }
    }

    /* Having a selectedNodeId && currently holding down left click means we want to drag only. */
    if (frameState_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT] && frameState_->clickedNodePtr != NO_PTR)
    {
        nodeevent::LMBDrag evt(x, y);
        frameState_->clickedNodePtr->getEvents().notifyAllChannels<nodeevent::LMBDrag>(evt);
        return;
    }
}

void WindowFrame::resolveOnWindowReizeFromInput(const int32_t newWidth, const int32_t newHeight)
{
    frameState_->isLayoutDirty = true;
    frameState_->frameSize = {newWidth, newHeight};
    for (const auto& node : allFrameChildNodes_)
    {
        node->onWindowResizeNotify();
    }
}
} // namespace msgui