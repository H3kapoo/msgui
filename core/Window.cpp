#include "Window.hpp"

#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

namespace msgui
{

GLXContext Window::sharedContext_ = {};
Display* Window::sharedDisplay_ = nullptr;
bool Window::uniqueContextAquired = false;

Window::Window(const std::string& windowName, const uint32_t width, const uint32_t height)
    : log_{"Window(" + windowName +")"}
    , windowName_(windowName)
    , width_(width)
    , height_(height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // move out
    // if (hints.debuggable)
    // {
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    // }
    windowHandle_ = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
    if (!windowHandle_)
    {
        log_.error("Failed to create!");
        glfwTerminate();
        return;
    }

    if (!uniqueContextAquired)
    {
        glfwMakeContextCurrent(windowHandle_);
        // glfwSwapInterval(0);
        // enableVSync();
        glEnable(GL_DEPTH_TEST);
        sharedDisplay_ = glfwGetX11Display();
        sharedContext_ = glXGetCurrentContext();
        uniqueContextAquired = true;
    }

    maskUnnecessaryEvents();
    log_.infoLn("Created!");

    onResizeEvent(width, height);
}

Window::~Window()
{
    log_.infoLn("Destroying window..");
    destroy();
}

void Window::setTitle(const std::string& title)
{
    glfwSetWindowTitle(windowHandle_, title.c_str());
    windowName_ = title; // this doesn't update the logger
}

void Window::setContextCurrent() const
{
    // glfwMakeContextCurrent(windowHandle_);
    glXMakeCurrent(sharedDisplay_, glfwGetX11Window(windowHandle_), sharedContext_);
}

void Window::setCurrentViewport() const
{
    glViewport(0, 0, width_, height_);
}

void Window::onResizeEvent(const uint32_t width, const uint32_t height)
{
    width_ = width;
    height_ = height;
    projMat_ = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -100.0f, 0.0f);

    setContextCurrent();
    setCurrentViewport();
}

void Window::swap() const
{
    glXSwapBuffers(sharedDisplay_, glfwGetX11Window(windowHandle_));
    // glfwSwapBuffers(windowHandle_);
}

void Window::destroy() const
{
    glfwDestroyWindow(windowHandle_);
}

GLFWwindow* Window::getHandle() const
{
    return windowHandle_;
}

std::string Window::getName() const
{
    return windowName_;
}

uint32_t Window::getWidth() const
{
    return width_;
}

uint32_t Window::getHeight() const
{
    return height_;
}

const glm::mat4& Window::getProjectionMat() const
{
    return projMat_;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(windowHandle_);
}

bool Window::initGlfwWindowing()
{
    return glfwInit();
}

void Window::enableVSync()
{
    glfwSwapInterval(1);
}

void Window::disableVSync()
{
    glfwSwapInterval(0);
}

void Window::terminate()
{
    glfwTerminate();
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::waitEvents()
{
    glfwWaitEvents();
}

void Window::clearColor(const glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Window::clearBits(const uint32_t bits)
{
    glClear(bits);
}

double Window::getTime()
{
    return glfwGetTime();
}

double Window::getFPS()
{
    return glfwGetTime();
}

void Window::maskUnnecessaryEvents()
{
    // On X11 systems when we have ONE context shared among MANY windows, the WM will spam out PropertyNotify
    // events endlessly. This messes with glfwWaitEvents() blocking nature and behaves just like a
    // glfwPollEvents() instead which is not what we want from a GUI app.
    // The way around this is to tell the WM to not generate PropertyNotify events anymore by masking the
    // attribute associated with that event.
    // Not sure if the behavior is similar on Windows/MacOS.

// #define PLATFORM_LINUX
    XWindowAttributes attributes;

    XGetWindowAttributes(sharedDisplay_, glfwGetX11Window(windowHandle_), &attributes);

    long current_mask = attributes.your_event_mask;
    long new_mask = current_mask & ~PropertyChangeMask;

    XSelectInput(sharedDisplay_, glfwGetX11Window(windowHandle_), new_mask);
}
} // namespace ecore