#include "Application.hpp"

#include <GLFW/glfw3.h>

#include "core/BELoadingQueue.hpp"
#include "core/Window.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/WindowFrame.hpp"

namespace msgui
{
Application::~Application()
{
    /* Release the frames in reverse order, just in case. terminate() needs to be called last */
    for (auto it = frames_.rbegin(); it != frames_.rend(); ++it)
    {
        it->reset();
    }

    initializationWindow_.reset();
    Window::terminate();
    log_.infoLn("App terminated..");
}

bool Application::init()
{
    /* Initialize glfw windowing */
    if (!Window::initGlfwWindowing())
    {
        log_.error("Failed to initialize GLFW Application!");
        return false;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    initializationWindow_ = std::make_shared<Window>("DummyWindow", 200, 200);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    if (!initializationWindow_->getHandle())
    {
        log_.error("Failed to create init window!");
        return false;
    }

    /* Dummy context is made current by default. */
    if (glewInit() != GLEW_OK)
    {
        return false;
    }

    log_.infoLn("Init successful!");
    return true;
}

void Application::run()
{
    static double previousTime = 0;
    static int32_t frameCount = 0;

    static double currentTime = glfwGetTime();
    static double delta = currentTime - previousTime;

    /* Only close the App if the primary window is closed. */
    while (!shouldAppClose_)
    {
        /* This queue will be populated with opengl resource load tasks from other threads and they will be
           resolved here in the main thread. This is because loading resources from different threads in
           opengl breaks the context they get assigned to. Delegating loading to the main thread bypasses
           any context invalidation shenenigans. Thread that requested resource loading will block until
           main thread finishes loading the resource in. If the resource is already in memory or trying to be
           loaded from the main thread, we will not reach this. */
        BELoadingQueue::get().executeTasks();

        std::erase_if(frames_,
            [this](const WindowFramePtr& frame)
            {
                currentTime = glfwGetTime();
                delta = currentTime - previousTime;
                frameCount++;

                if (frame->isPrimary() && delta > 1.0f)
                {
                    FPS_ = frameCount / delta;
                    frame->window_.setTitle(std::to_string(FPS_));

                    frameCount = 0;
                    previousTime = currentTime;
                }

                bool shallFrameClose = frame->run();
                if (shallFrameClose && frame->isPrimary())
                {
                    shouldAppClose_ = true;
                }

                return shallFrameClose;
            });

        if (shouldAppClose_)
        {
            break;
        }

        /* Note: Event solvers (like callbacks) are processed before rendering & updating the layout */
        pollMode_ == PollMode::ON_EVENT ? Window::waitEvents() : Window::pollEvents();
    }
}

WindowFramePtr& Application::createFrame(const std::string& windowName, const uint32_t width, const uint32_t height)
{
    const bool isPrimary = frames_.empty();
    auto newFrame = std::make_shared<WindowFrame>(windowName, width, height, isPrimary);
    return frames_.emplace_back(newFrame);
}

void Application::setPollMode(const PollMode mode)
{
    pollMode_ = mode;
}

void Application::setVSync(const bool vsyncValue)
{
    Window::setVSync(vsyncValue);
}

WindowFramePtr Application::getFrameId(const uint32_t id)
{
    const auto it = std::find_if(frames_.begin(), frames_.end(),
        [&id](const WindowFramePtr& frame)
        {
            return frame->getRoot()->getId() == id;
        });

    if (it == frames_.end())
    {
        return nullptr;
    }

    return *it;
}

Application& Application::get()
{
    static Application app;
    return app;
}
} // namespace msgui