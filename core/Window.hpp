#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <GL/glx.h>

#include <glm/glm.hpp>

#include "Logger.hpp"

namespace msgui
{

class Window
{
public:
    Window(const std::string& windowName, const uint32_t width, const uint32_t height);
    ~Window();

    void setTitle(const std::string& title);
    void setContextCurrent() const;
    void setCurrentViewport() const;
    void onResizeEvent(const uint32_t width, const uint32_t height);
    void swap() const;
    void destroy() const;
    void setEventBits(const uint16_t enabledEventBits);
    void unsetEventBits(const uint16_t unsetEventBits);
    bool shouldClose() const;

    GLFWwindow* getHandle() const;
    std::string getName() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    const glm::mat4& getProjectionMat() const;

    static double getTime();
    static double getFPS();

    static bool initGlfwWindowing();
    static void enableVSync();
    static void disableVSync();
    static void terminate();
    static void pollEvents();
    static void waitEvents();
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

public:
    static GLXContext sharedContext_;
    static Display* sharedDisplay_;
    static bool uniqueContextAquired;
};
using WindowPtr = std::shared_ptr<Window>;
} // namespace msgui
