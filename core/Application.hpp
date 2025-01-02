#pragma once

#include <list>

#include "core/Logger.hpp"
#include "core/node/Frame.hpp"
#include "core/Window.hpp"

namespace msgui
{
class Application
{
public:
    Application() = default;
    ~Application();

    // Normal
    bool init();
    void run();
    FramePtr createFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);

    // Getters
    FramePtr getFrameId(const uint32_t id);
    // bool hasFrameId(const uint32_t id);

    // Statics
    static Application& get();

private:
    Logger log_{"Application"};
    WindowPtr initializationWindow_;

    // List references remain valid even after addition/removal and in this case, because of that, it is
    // better to use lists instead of vectors.
    std::list<FramePtr> frames_;
    bool shouldAppClose_{false};
    int32_t FPS_{0};
};
} // namespace msgui