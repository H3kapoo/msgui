#include "Window.hpp"

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
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    windowHandle_ = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
    if (!windowHandle_)
    {
        log_.error("Failed to create! Check if App was initialized properly!");
        glfwTerminate();
        return;
    }

    if (!uniqueContextAquired)
    {
        glfwMakeContextCurrent(windowHandle_);
        setVSync(1);
        setDepthTest(true);
        setScissorTest(true);
        // setBlending(true);
        sharedDisplay_ = glfwGetX11Display();
        sharedContext_ = glXGetCurrentContext();
        uniqueContextAquired = true;
    }

    maskUnnecessaryEvents();
    onResizeEvent(width, height);
    log_.infoLn("Created!");
}

Window::~Window()
{
    log_.infoLn("Destroying window..");
    destroy();
}

void Window::swap() const
{
    glXSwapBuffers(sharedDisplay_, glfwGetX11Window(windowHandle_));
}

void Window::destroy() const
{
    glfwDestroyWindow(windowHandle_);
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(windowHandle_);
}

void Window::onResizeEvent(const uint32_t width, const uint32_t height)
{
    width_ = width;
    height_ = height;

    /* Normally the Z axis in opengl points to the negative Z (into the screen).
       Reverse & flip sign of Z_near and Z_Far so that elements can be placed from
       [+Z_near, +Z_far] which lower Z appearing further into the camera and higher Z
       closer to the camera, mimicing layers.
       If not for this, we would of had to place elements with a negative Z and the highest Z
       (e.g -1) would of appeared in front of lowest Z (e.g -100) which is not what we want. */
    projMat_ = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -(float)MAX_LAYERS, 0.0f);

    setContextCurrent();
    setCurrentViewport();
    setCurrentScissorArea();
}

void Window::setTitle(const std::string& title)
{
    glfwSetWindowTitle(windowHandle_, title.c_str());
    windowName_ = title; /* Note: this doesn't update the logger */
}

void Window::setContextCurrent() const
{
    glXMakeCurrent(sharedDisplay_, glfwGetX11Window(windowHandle_), sharedContext_);
}

void Window::setCurrentViewport() const
{
    glViewport(0, 0, width_, height_);
}

void Window::setCurrentScissorArea() const
{
    setScissorArea(0, 0, width_, height_);
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

bool Window::initGlfwWindowing()
{
    return glfwInit();
}

void Window::setBlending(const bool state)
{
    /* Note: A context needs to be bound for this to work */
    state ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::setDepthTest(const bool state)
{
    state ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Window::setScissorTest(const bool state)
{
    state ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}

void Window::setScissorArea(const int32_t x, const int32_t y, const int32_t width, const int32_t height)
{
    glScissor(x, y, width, height);
}

void Window::setVSync(const int32_t internal)
{
#ifdef __linux__
    typedef int (*PFNGLXSWAPINTERVALMESAPROC)(unsigned int);
    PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = reinterpret_cast<PFNGLXSWAPINTERVALMESAPROC>(
        glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalMESA")));
    if (glXSwapIntervalMESA)
    {
         glXSwapIntervalMESA(internal);
    }

#else
    glfwSwapInterval(internal);
#endif
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

void Window::requestEmptyEvent()
{
    glfwPostEmptyEvent();
}

void Window::clearColor(const glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Window::clearBits(const uint32_t bits)
{
    glClear(bits);
}

void Window::maskUnnecessaryEvents()
{
    /* On X11 systems when we have ONE context shared among MANY windows, the WM will spam out PropertyNotify
       events endlessly. This messes with glfwWaitEvents() blocking nature and behaves just like a
       glfwPollEvents() instead which is not what we want from a GUI app.
       The way around this is to tell the WM to not generate PropertyNotify events anymore by masking the
       attribute associated with that event.
       Not sure if the behavior is similar on Windows/MacOS. */

// #define PLATFORM_LINUX
    XWindowAttributes attributes;

    XGetWindowAttributes(sharedDisplay_, glfwGetX11Window(windowHandle_), &attributes);

    long current_mask = attributes.your_event_mask;
    long new_mask = current_mask & ~PropertyChangeMask;

    XSelectInput(sharedDisplay_, glfwGetX11Window(windowHandle_), new_mask);
}
} // namespace ecore
