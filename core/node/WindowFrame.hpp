#pragma once

#include <cstdint>
#include <memory>

#include "core/layoutEngine/ILayoutEngine.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
class Application;

/* UI Window content manager */
class WindowFrame
{
/*  Since scrollbars need to be displayed on top of the container they attach to and kinda on top of
    everything else, a custom Z start is needed. */
static constexpr uint32_t SCROLL_LAYER_START = 250;

public:
    /**
        Creates a new window frame.

        @param windowName Name of the window
        @param width Desired window width
        @param height Desired window height
        @param isPrimary Specifies if this is the primary (main) window
    */
    WindowFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);
    ~WindowFrame();

    /**
        Takes a screenshot of the current pixels buffers and saves it to a file.

        @param filePath Filepath to save to (as jpg)
        @param quality JPG quality (0 to 100)
    */
    void saveBufferToFile(const std::string& filePath, const int32_t quality = 100) const;

    /**
        Get underlying root node

        @return Pointer to box container node
    */
    BoxPtr getRoot();

    /**
        Quickly check if this window is the primary one.

        @return Is primary or not
    */
    bool isPrimary() const;

private: // friend
    friend Application;

    /**
        Runs the window update loop.

        @return True if window should close now
     */
    bool run();

private:
    void renderLayout();
    void updateLayout();
    void resolveNodeRelations();

    void resolveOnMouseButtonFromInput(const int32_t btn, const int32_t action);
    void resolveOnMouseMoveFromInput(const int32_t x, const int32_t y);
    void resolveOnWindowReizeFromInput(const int32_t newWidth, const int32_t newHeight);

private:
    Logger log_;
    Window window_;
    Input input_;
    FrameStatePtr frameState_{nullptr};
    bool shouldWindowClose_{false};
    ILayoutEnginePtr layoutEngine_{nullptr}; // this should be a singleton maybe as it doesnt depend on anything
    std::vector<AbstractNodePtr> allFrameChildNodes_;
    BoxPtr frameBox_{nullptr};
    bool isPrimary_{false};

    static std::array<GLFWcursor*, 6> standardCursors_;
    static bool initCursors;
};
using WindowFramePtr = std::shared_ptr<WindowFrame>;
} // msgui