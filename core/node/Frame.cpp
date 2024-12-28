#include "Frame.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <queue>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/Window.hpp"
#include "core/Renderer.hpp"
#include "core/node/AbstractNode.hpp"

namespace msgui
{
Frame::Frame(const std::string& windowName, const uint32_t width, const uint32_t height)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"), windowName)
    , window_(windowName, width, height)
    , input_(&window_)
{
    log_ = ("Frame(" + windowName + ")");

    input_.onRefresh([this]()
    {
        log_.infoLn("refresh called?");
    });

    input_.onWindowResize([this](uint32_t width, uint32_t height)

    {
        window_.setTitle(std::to_string(width) + " " + std::to_string(height));
        log_.infoLn("on resize");
        window_.onResizeEvent(width, height);
        getTransform().setScale({width, height, 1});
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

    state_ = std::make_shared<FrameState>();
    getTransform().setPos({0, 0, 1});
    getTransform().setScale({width, height, 1});
}

void Frame::renderLayout()
{   
    // Currently we need front to back to minimize overdraw by making use of the depth buffer.
    // Not sure how this will work when we need alpha blending between nodes. Maybe we will need
    // to render back to front in that case and take overdrawing as a compromise.
    for (auto& node : allFrameChildNodes_)
    {
        Renderer::render(node.get(), window_.getProjectionMat());
    }
    Renderer::render(this, window_.getProjectionMat());
}

void Frame::updateLayout()
{
    // Must redo internal vector structure if something was added/removed
    if (state_->isLayoutDirty)
    {
        log_.infoLn("Layout is dirty");
        state_->isLayoutDirty = false;

        resolveNodeRelations();
    }
}

void Frame::resolveNodeRelations()
{
    allFrameChildNodes_.clear();

    std::queue<AbstractNodePtr> q;

    for (const auto& ch : children_)
    {
        // Set frames' children frameState and depth
        ch->depth_ = depth_ + 1;
        ch->state_ = state_;
        q.push(ch);
    }

    while (!q.empty())
    {
        AbstractNodePtr node = q.front();
        q.pop();
        allFrameChildNodes_.push_back(node);

        for (auto& ch : node->getChildren())
        {
            // Set children's frameState and depth
            ch->parent_ = node;
            ch->depth_ = node->getDepth() + 1;
            ch->state_ = state_;
            q.push(ch);
        }
    }

    // Sort nodes from high to low depth
    std::ranges::sort(allFrameChildNodes_, [](const AbstractNodePtr a, const AbstractNodePtr b)
    {
        return a->getDepth() > b->getDepth();
    });
}

void Frame::resolveOnMouseButtonFromInput(int32_t btn, int32_t action)
{
    // log_.debug("button is %d action %d", btn, action);
    state_->mouseButtonState[btn] = action;
    for (const auto& node : allFrameChildNodes_)
    {
        node->onMouseButtonNotify();
    }
    onMouseButtonNotify();
}

bool Frame::run()
{
    // layout pass
    updateLayout();

    // render pass
    window_.setContextCurrent();
    window_.setCurrentViewport();
    // if (window_.getName() == "Window2")
    // {
        Window::clearColor(glm::vec4{0.0, 1.0, 0.0, 1.0f});
    // }
    // else
    // {
    //     Window::clearColor(glm::vec4{1.0, 0.0, 0.0, 1.0f});
    // }
    Window::clearBits(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderLayout();

    window_.swap();

    return shouldWindowClose_ || window_.shouldClose();
}

void Frame::setShaderAttributes()
{
    // AbstractNode::setShaderAttributes();
    transform_.computeModelMatrix();
    // shader_->setTexture2D("uTexture", GL_TEXTURE0, btnTex->getId());
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}
};