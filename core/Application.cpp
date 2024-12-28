#include "Application.hpp"

#include "core/Window.hpp"
#include <GLFW/glfw3.h>

namespace msgui
{

Application& Application::get()
{
    static Application app;
    return app;
}

Application::Application()
{}

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

    // Dummy context is made current by default.
    if (glewInit() != GLEW_OK)
    {
        return false;
    }

    log_.infoLn("Init successful!");
    return true;
}

void Application::attachFrame(const FramePtr frame)
{
    frames_.push_back(frame);
}

void Application::run()
{
    bool shouldAppClose{false};
    while (!shouldAppClose)
    {
        for (const auto& frame : frames_)
        {
            // shouldAppClose = frame->run() || frame->isPrimaryFrame();
            if (frame->run())
            {
                shouldAppClose = true;
            }
            // Window::pollEvents();
        }

        if (shouldAppClose)
        {
            break;
        }

        Window::waitEvents();
        log_.infoLn("pe aici");
    }
}
} // namespace msgui