#pragma once

#include <cstdint>
#include <memory>
#include <string>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef __linux__
#include <GL/glx.h>
#endif

#include <glm/glm.hpp>

#include "Logger.hpp"

namespace msgui
{
/* Manages creation & customization of actual OS windows. */
class Window
{
static constexpr uint32_t MAX_LAYERS = 1000;

public:
    /**
        Creates a new OS window.

        @param windowName Name of the window
        @param width Desired window width
        @param height Desired window height
    */
    Window(const std::string& windowName, const uint32_t width, const uint32_t height);
    ~Window();

    /**
        Swap back and front buffers.
    */
    void swap() const;

    /**
        Destroy this window.
    */
    void destroy() const;

    /**
        Check if close this window event was triggered (Clicking X for example).
    */
    bool shouldClose() const;

    /**
        Handle window resizing. This updates scissors area and viewport area.

        @param width New window width
        @param height New window height
    */
    void onResizeEvent(const uint32_t width, const uint32_t height);

    /* Trivial setters */
    void setTitle(const std::string& title);
    void setContextCurrent() const;
    void setCurrentViewport() const;
    void setCurrentScissorArea() const;
    void setVSync(const int32_t interval) const;

    /* Trivial getters */
    GLFWwindow* getHandle() const;
    std::string getName() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    const glm::mat4& getProjectionMat() const;

    /**
        Initialize GLFW library
    */
    static bool initGlfwWindowing();

    /* Trivial static setters */
    static void setBlending(const bool state);
    static void setDepthTest(const bool state);
    static void setScissorTest(const bool state);
    static void setScissorArea(const int32_t x, const int32_t y, const int32_t width, const int32_t height);
    static GLFWwindow* getSharedContexWindowHandle();

    /**
        Terminate GLFW library
    */
    static void terminate();

    /**
        Poll events in the queue.
    */
    static void pollEvents();

    /**
        Wait for events to appear in the queue.
    */
    static void waitEvents();

    /**
        Manually append empty event in the queue.
    */
    static void requestEmptyEvent();

    /**
        Clear window with color.

        @param color Color to clear with
    */
    static void clearColor(const glm::vec4 color);

    /**
        Clear window bits.

        @param bits Bits to clear (color/depth/stencil etc)
    */
    static void clearBits(const uint32_t bits);

private:
    void setupEventCallbacks();
    void maskUnnecessaryEvents();

private:
    Logger log_;
    GLFWwindow* windowHandle_{nullptr};
    std::string windowName_;
    uint32_t width_{0};
    uint32_t height_{0};
    glm::mat4 projMat_{glm::mat4(1.0f)};

    static GLFWwindow* sharedWindowHandle_;
    static GLXContext sharedContext_;
    static Display* sharedDisplay_;
    static bool uniqueContextAquired;
};
using WindowPtr = std::shared_ptr<Window>;
} // namespace msgui
