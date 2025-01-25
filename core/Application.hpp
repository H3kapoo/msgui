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

    enum class Toggle
    {
        OFF = 0,
        ON  = 1
    };

public:

    bool init();
    void run();
    WindowFramePtr createFrame(const std::string& windowName, const uint32_t width, const uint32_t height);

    void setPollMode(const PollMode mode);
    void setVSync(const Toggle toggle);

    WindowFramePtr getFrameId(const uint32_t id);

    static Application& get();

private:
    Application() = default;
    Application(Application&&) = delete;
    Application(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    ~Application();

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