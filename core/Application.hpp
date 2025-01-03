#pragma once

#include <list>

#include "core/Logger.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/Window.hpp"

namespace msgui
{
class Application
{
public:
    enum class PollMode
    {
        ON_EVENT   = 0,
        CONTINUOUS = 1
    };

public:
    Application() = default;
    ~Application();

    bool init();
    void run();
    WindowFramePtr createFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);

    void setPollMode(const PollMode mode);

    WindowFramePtr getFrameId(const uint32_t id);

    static Application& get();

private:
    Logger log_{"Application"};
    WindowPtr initializationWindow_;

    // List references remain valid even after addition/removal and in this case, because of that, it is
    // better to use lists instead of vectors.
    std::list<WindowFramePtr> frames_;
    bool shouldAppClose_{false};
    int32_t FPS_{0};
    PollMode pollMode_{PollMode::ON_EVENT};
};
} // namespace msgui