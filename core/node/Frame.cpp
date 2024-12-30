#include "Frame.hpp"

#include <algorithm>
#include <ranges>
#include <queue>

#include <GLFW/glfw3.h>

#include "core/layoutEngine/SimpleLayoutEngine.hpp"
#include "core/Renderer.hpp"

namespace msgui
{
Frame::Frame(const std::string& windowName, const uint32_t width, const uint32_t height)
    : log_("Frame(" + windowName + ")")
    , window_(windowName, width, height)
    , input_(&window_)
    , frameState_(std::make_shared<FrameState>())
    , layoutEngine_(std::make_shared<SimpleLayoutEngine>())
    , frameBox_(std::make_shared<Box>(log_.getName()))
{
    input_.onWindowResize([this](uint32_t width, uint32_t height)
    {
        window_.setTitle(std::to_string(width) + " " + std::to_string(height));
        window_.onResizeEvent(width, height);
        frameBox_->transform_.setScale({width, height, 1});
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

    frameBox_->setColor({204/255.0f, 51/255.0f, 139/255.0f, 1.0f});
    frameBox_->transform_.setPos({0, 0, 1});
    frameBox_->transform_.setScale({width, height, 1});
    frameBox_->state_ = frameState_;
}

// ---- Normal ---- //
AbstractNodePtr Frame::getRoot()
{
    return frameBox_;
}

// ---- Normal Private ---- //
bool Frame::run()
{
    // layout pass
    updateLayout();

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
    if (frameState_->isLayoutDirty)
    {
        // log_.infoLn("Layout is dirty");
        frameState_->isLayoutDirty = false;

        resolveNodeRelations();

        // // iterate from lowest depth to highest
        for (auto& node : allFrameChildNodes_ | std::views::reverse)
        {
            layoutEngine_->process(node);
        }
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
            // Set children's frameState and depth
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

void Frame::resolveOnMouseButtonFromInput(int32_t btn, int32_t action)
{
    // log_.debug("button is %d action %d", btn, action);
    frameState_->mouseButtonState[btn] = action;
    for (const auto& node : allFrameChildNodes_)
    {
        node->onMouseButtonNotify();
    }
}
} // namespace msgui