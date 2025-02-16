#pragma once

#include <list>

#include "msgui/Logger.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/Window.hpp"

namespace msgui
{
class Application
{
public:
    /* Poll mode for the application. App can run the redering logic CONTINOUSLY or only ON_EVENT triggered by user
       such as mose movement, window resizing, etc. It's recommended to use ON_EVENT to prevent busy looping. */
    enum class PollMode { ON_EVENT = 0, CONTINUOUS = 1 };

public:
    /**
        Initializes application dependencies.
        Note: Mandatory function to be called before doing ANYTHING!

        @return True on success and False on failure
    */
    bool init();

    /**
        Runs the application loop.
        Note: Blocking operation. This will return only on main window close request.
     */
    void run();

    /**
        Create a window frame to append elements into.

        @note Do not store it as a pointer. Store and use it as a reference.

        @param windowName Name of the window
        @param width Desired window width
        @param height Desired window height

        @return Reference to the created window frame
     */
    WindowFramePtr& createFrame(const std::string& windowName, const uint32_t width, const uint32_t height);

    /**
        Sets the poll mode of the application.

        @param Poll mode
    */
    void setPollMode(const PollMode mode);

    /**
        Sets vsync to enabled or disabled.

        @param vsyncValue True for ON, False for OFF
    */
    void setVSync(const bool vsyncValue);

    /**
        Find and return window frame with specific id.

        @note Do not store it. Use it and let go.

        @param id Id of the window root box

        @return Pointer to the window frame
     */
    WindowFramePtr getFrameId(const uint32_t id);

    /**
        Find and return window frame with specific name.

        @note Do not store it. Use it and let go.

        @param name Name of the window root box

        @return Pointer to the window frame
     */
    WindowFramePtr getFrameNamed(const std::string& name);

    /**
        Get Application instace.

        @return Reference to application instace
     */
    static Application& get();

private:
    /* No copies or moves allowed. */
    Application() = default;
    Application(Application&&) = delete;
    Application(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    ~Application();

    Logger log_{"Application"};
    WindowPtr initializationWindow_;
    std::list<WindowFramePtr> frames_;
    bool shouldAppClose_{false};
    int32_t FPS_{0};
    PollMode pollMode_{PollMode::ON_EVENT};
};
} // namespace msgui