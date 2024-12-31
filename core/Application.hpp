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

    FrameUPtr& createFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);
    FrameUPtr* getFrameId(const uint32_t id);

    // void attachFrame(const FramePtr& frame);
    void run();

    // Statics
    static Application& get();

private:
    Logger log_{"Application"};
    WindowPtr initializationWindow_;

    // List references remain valid even after addition/removal and in this case, because of that, it is
    // better to use lists instead of vectors.
    std::list<FrameUPtr> frames_;
    bool shouldAppClose_{false};
    int32_t FPS_{0};
};
} // namespace msgui