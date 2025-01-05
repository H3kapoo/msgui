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
    Window(const std::string& windowName, const uint32_t width, const uint32_t height);
    ~Window();

    void swap() const;
    void destroy() const;
    bool shouldClose() const;

    void onResizeEvent(const uint32_t width, const uint32_t height);

    void setTitle(const std::string& title);
    void setContextCurrent() const;
    void setCurrentViewport() const;

    GLFWwindow* getHandle() const;
    std::string getName() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    const glm::mat4& getProjectionMat() const;

    static bool initGlfwWindowing();
    static void setBlending(const bool state);
    static void setDepthTest(const bool state);
    static void setScissorTest(const bool state);
    static void setVSync(const int32_t interval);
    static void terminate();
    static void pollEvents();
    static void waitEvents();
    static void requestEmptyEvent();
    static void clearColor(const glm::vec4 color);
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

    static GLXContext sharedContext_;
    static Display* sharedDisplay_;
    static bool uniqueContextAquired;
};
using WindowPtr = std::shared_ptr<Window>;
} // namespace msgui
