#pragma once


#include "core/Logger.hpp"
#include "core/node/Frame.hpp"
#include "core/Window.hpp"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <stdio.h>


namespace msgui
{
class Application
{
public:
    static Application& get();

    Application();
    ~Application();

    bool init();
    void attachFrame(const FramePtr frame);
    void run();

private:
    Logger log_{"Application"};

    WindowPtr initializationWindow_;
    std::vector<FramePtr> frames_;
};
} // namespace msgui