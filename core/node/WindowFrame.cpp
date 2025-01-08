#include "WindowFrame.hpp"

#include <algorithm>
#include <memory>
#include <ranges>
#include <queue>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "core/vendor/stb_image_write.h"

#include "core/layoutEngine/SimpleLayoutEngine.hpp"
#include "core/Renderer.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
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
        window_.setTitle(std::to_string(newWidth) + " " + std::to_string(newHeight));
        window_.onResizeEvent(newWidth, newHeight);
        frameBox_->transform_.scale = {newWidth, newHeight, 1};
        frameBox_->transform_.vScale = {newWidth, newHeight};
        frameState_->isLayoutDirty = true;
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

    frameBox_->props.color = Utils::hexToVec4("#cc338bff");
    frameBox_->transform_.pos = {0, 0, 1};
    frameBox_->transform_.scale = {width, height, 1};
    frameBox_->transform_.vPos = {0, 0};
    frameBox_->transform_.vScale = {width, height};
    frameBox_->state_ = frameState_;
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
    // layout pass
    if (frameState_->isLayoutDirty)
    {
        // It's important for the variable to be reset before the layout update is called as layoutUpdate
        // can SET the variable to true again if it decides the layout got dirty.
        frameState_->isLayoutDirty = false;
        updateLayout();

        // If the layout got dirty again we need to simulate a new frame RUN request.
        if (frameState_->isLayoutDirty)
        {
            Window::requestEmptyEvent();
            //TODO: Theoretically it's better to use run() here but it can get stuck in an infinite
            // layout calculation loop if we mess pretty hard with the window size.
            // Calling requestEmptyEvent is a bit slower because it needs to also render the frame but maybe
            // in practice there's not a big penalty.
            // See in the future if infinite looping can be mitigated.
            // run();
        }
    }

    // render pass
    window_.setContextCurrent();
    window_.setCurrentViewport();
    Window::clearColor(glm::vec4{0.0, 1.0, 0.0, 1.0f});
    Window::clearBits(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderLayout();
    
    window_.swap();

    return shouldWindowClose_ || window_.shouldClose();
}

void WindowFrame::renderLayout()
{
    const auto pMat = window_.getProjectionMat();

    // Currently we need front to back to minimize overdraw by making use of the depth buffer.
    // Not sure how this will work when we need alpha blending between nodes. Maybe we will need
    // to render back to front in that case and take overdrawing as a compromise.
    // TODO: Deal with transparent objects. Current fix is to render back to front (reverse) when there are
    // transparent objects.
    // for (auto& node : allFrameChildNodes_ | std::views::reverse) // -> back to front Z
    for (auto& node : allFrameChildNodes_) // -> front to back Z
    {
        Renderer::render(node, pMat);
    }
}

void WindowFrame::updateLayout()
{
    // Must redo internal vector structure if something was added.
    // Removal preserves the node "depth"-ness so we don't need to redo it.
    if (frameState_->layoutStoreNeedsRecreate)
    {
        resolveNodeRelations();
    }

    // Iterate from lowest depth to highest
    for (auto& node : allFrameChildNodes_ | std::views::reverse)
    {
        glm::ivec2 overflow = layoutEngine_->process(node);
        // Currently only BOX type nodes support overflow handling
        if (node->getType() == AbstractNode::NodeType::BOX)
        {
            static_cast<Box*>(node.get())->updateOverflow(overflow);
        }

        // After updating the node layout, we need to update the viewable area of the node. Raw parent is used
        // for better performance (compared to locking each time).
        if (auto p = node->getParentRaw())
        {
            node->transform_.computeViewableArea(p->transform_);
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
            // Set children's frameState and depth if needed
            if (!ch->state_)
            {
                bool isScrollNode = ch->getType() == AbstractNode::NodeType::SCROLL;
                ch->parent_ = node;
                ch->parentRaw_ = node.get();
                ch->transform_.pos.z = node->transform_.pos.z + (isScrollNode ? SCROLL_LAYER_START : 1);
                ch->state_ = frameState_;
            }

            q.push(ch);
        }
    }

    // Sort only if necessary to speed-up things
    // if (frameState_->layoutNeedsSort)
    {
        // Sort nodes from high to low depth
        std::ranges::sort(allFrameChildNodes_,
            [](const AbstractNodePtr a, const AbstractNodePtr b)
            {
                return a->getTransform().pos.z > b->getTransform().pos.z;
            });
        frameState_->layoutNeedsSort = false;
    }
}

void WindowFrame::resolveOnMouseButtonFromInput(const int32_t btn, const int32_t action)
{
    // log_.debug("button is %d action %d", btn, action);
    frameState_->mouseButtonState[btn] = action;
    frameState_->lastMouseButtonTriggeredIdx = btn;

    int32_t mX = frameState_->mouseX;
    int32_t mY = frameState_->mouseY;
    for (const auto& node : allFrameChildNodes_)
    {
        // Ignore clicks on SCROLL_KNOB. SCROLL_KNOB is NOT draggable directly.
        if (node->getType() == AbstractNode::NodeType::SCROLL_KNOB) { continue; }

        // TODO: We shall not use absolute node values but the computed "viewable node area"
        // after node scissoring with it's parent. Otherwise we can click the child node that's
        // visually outside of it's parent.
        glm::vec3& nodePos = node->transform_.pos;
        glm::vec3& nodeScale = node->transform_.scale;
        if ((mX >= nodePos.x && mX <= nodePos.x + nodeScale.x) &&
            (mY >= nodePos.y && mY <= nodePos.y + nodeScale.y))
        {
            // If we got here by pressing the mouse button, this is the new selected node.
            if (frameState_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
            {
                frameState_->clickedNodePtr = node;
            }
            // Otherwise we need to clear whatever we deduced to be pressed before
            else
            {
                frameState_->clickedNodePtr = NO_PTR;
            }

            node->onMouseButtonNotify();
            break; // event was consumed
        }
    }
}

void WindowFrame::resolveOnMouseMoveFromInput(const int32_t x, const int32_t y)
{
    frameState_->mouseX = x;
    frameState_->mouseY = y;

    // Having a selectedNodeId && currently holding down left click means we want to drag only.
    if (frameState_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT] && frameState_->clickedNodePtr != NO_PTR)
    {
        frameState_->clickedNodePtr->onMouseDragNotify();
        return;
    }

    // Resolve hovered item
    for (const auto& node : allFrameChildNodes_)
    {
        // TODO: We shall not use absolute node values but the computed "viewable node area"
        // after node scissoring with it's parent. Otherwise we can click the child node that's
        // visually outside of it's parent.
        glm::vec3& nodePos = node->transform_.pos;
        glm::vec3& nodeScale = node->transform_.scale;
        if ((x >= nodePos.x && x <= nodePos.x + nodeScale.x) &&
            (y >= nodePos.y && y <= nodePos.y + nodeScale.y))
        {
            node->onMouseHoverNotify();
            break; // event was consumed
        }
    }
}
} // namespace msgui