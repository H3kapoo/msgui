#include "Frame.hpp"

#include <algorithm>
#include <memory>
#include <ranges>
#include <queue>

#include <GLFW/glfw3.h>

#include "core/layoutEngine/SimpleLayoutEngine.hpp"
#include "core/Renderer.hpp"

namespace msgui
{
Frame::Frame(const std::string& windowName, const uint32_t width, const uint32_t height, const bool isPrimary)
    : log_("Frame(" + windowName + ")")
    , window_(windowName, width, height)
    , input_(&window_)
    , frameState_(std::make_shared<FrameState>())
    , layoutEngine_(std::make_shared<SimpleLayoutEngine>())
    , frameBox_(std::make_shared<Box>(windowName))
    , isPrimary_(isPrimary)
{
    input_.onWindowResize([this](uint32_t width, uint32_t height)
    {
        window_.setTitle(std::to_string(width) + " " + std::to_string(height));
        window_.onResizeEvent(width, height);
        frameBox_->transform_.setScale({width, height, 1});
        frameState_->isLayoutDirty = true;
    });

    input_.onKeyPress([this](int32_t key, int32_t scanCode, int32_t mods)
    {
        if (key == GLFW_KEY_Q)
        {
            log_.infoLn("Quit called");
            shouldWindowClose_ = true;
        }
    });

    input_.onMouseButton(
        std::bind(
            &Frame::resolveOnMouseButtonFromInput,
            this, std::placeholders::_1, std::placeholders::_2));
    
    input_.onMouseMove(
        std::bind(
            &Frame::resolveOnMouseMoveFromInput,
            this, std::placeholders::_1, std::placeholders::_2));

    frameBox_->setColor({204/255.0f, 51/255.0f, 139/255.0f, 1.0f});
    frameBox_->transform_.setPos({0, 0, 1});
    frameBox_->transform_.setScale({width, height, 1});
    frameBox_->state_ = frameState_;

    // temp
    // frameBox_->enableVScroll();
    // window_.setContextCurrent();
    // window_.disableVSync();
}

// ---- Normal ---- //
AbstractNodePtr Frame::getRoot()
{
    return frameBox_;
}

// ---- Getters ---- //
bool Frame::isPrimary() const
{
    return isPrimary_;
}

// ---- Normal Private ---- //
bool Frame::run()
{
    // layout pass
    if (frameState_->isLayoutDirty)
    {
        updateLayout();
        frameState_->isLayoutDirty = false;
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

void Frame::renderLayout()
{   
    // Currently we need front to back to minimize overdraw by making use of the depth buffer.
    // Not sure how this will work when we need alpha blending between nodes. Maybe we will need
    // to render back to front in that case and take overdrawing as a compromise.
    const auto pMat = window_.getProjectionMat();
    for (auto& node : allFrameChildNodes_)
    {
        Renderer::render(node, pMat);
    }
}

void Frame::updateLayout()
{
    // Must redo internal vector structure if something was added/removed
    resolveNodeRelations();

    // Iterate from lowest depth to highest
    for (auto& node : allFrameChildNodes_ | std::views::reverse)
    {
        layoutEngine_->process(node);
    }
}

void Frame::resolveNodeRelations()
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
                ch->parent_ = node;
                ch->transform_.pos.z = node->transform_.pos.z + 1;
                ch->state_ = frameState_;
            }
            q.push(ch);
        }
    }

    // Sort nodes from high to low depth
    std::ranges::sort(allFrameChildNodes_,
        [](const AbstractNodePtr a, const AbstractNodePtr b)
        {
            return a->getTransform().pos.z > b->getTransform().pos.z;
        });
}

// ---- Window Input Resolvers ---- //
void Frame::resolveOnMouseButtonFromInput(int32_t btn, int32_t action)
{
    // log_.debug("button is %d action %d", btn, action);
    frameState_->mouseButtonState[btn] = action;
    frameState_->lastMouseButtonTriggeredIdx = btn;

    int32_t mX = frameState_->mouseX;
    int32_t mY = frameState_->mouseY;
    for (const auto& node : allFrameChildNodes_)
    {
        // Ignore clicks on SCROLL_KNOB
        if (node->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        {
            continue;
        }

        glm::vec3& nodePos = node->transform_.pos;
        glm::vec3& nodeScale = node->transform_.scale;
        if ((mX >= nodePos.x && mX <= nodePos.x + nodeScale.x) &&
            (mY >= nodePos.y && mY <= nodePos.y + nodeScale.y))
        {
            node->onMouseButtonNotify();
            break; // event was consumed
        }
    }
}

void Frame::resolveOnMouseMoveFromInput(int32_t x, int32_t y)
{
    frameState_->mouseX = x;
    frameState_->mouseY = y;
}

} // namespace msgui