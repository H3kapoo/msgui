#include "Application.hpp"

#include <GLFW/glfw3.h>

#include "core/Window.hpp"
#include "core/node/Frame.hpp"

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

// ---- Normal ---- //
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

FrameUPtr& Application::createFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
    const bool isPrimary)
{
    return frames_.emplace_back(std::make_unique<Frame>(windowName, width, height, isPrimary));
}

FrameUPtr* Application::getFrameId(const uint32_t id)
{
    const auto it = std::find_if(frames_.begin(), frames_.end(),
        [&id](const FrameUPtr& frame)
        {
            return frame->getRoot()->getId() == id;
        });
    
    if (it == frames_.end())
    {
        return nullptr;
    }

    return &(*it);
}

void Application::run()
{
    static double previousTime = 0;
    static int32_t frameCount = 0;

    double currentTime = glfwGetTime();
    double delta = currentTime - previousTime;

    // Only close the App if the primary window is closed.
    while (!shouldAppClose_)
    {
        std::erase_if(frames_,
            [this, &delta, &currentTime](const FrameUPtr& frame)
            {
                currentTime = glfwGetTime();
                delta = currentTime - previousTime;
                frameCount++;

                if (frame->isPrimary() && delta > 1.0f)
                {
                    FPS_ = frameCount / delta;
                    // frame->window_.setTitle("FPS: " + std::to_string(FPS_));
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

        // Window::pollEvents();
        Window::waitEvents();
    }
}

// ---- Statics ---- //
Application& Application::get()
{
    static Application app;
    return app;
}

} // namespace msgui