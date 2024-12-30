#pragma once

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
    void attachFrame(const FramePtr frame);
    void run();

    // Statics
    static Application& get();

private:
    Logger log_{"Application"};

    WindowPtr initializationWindow_;
    std::vector<FramePtr> frames_;
};
} // namespace msgui