#pragma once

#include <cstdint>
#include <memory>

#include "msgui/layoutEngine/ILayoutEngine.hpp"
#include "msgui/Window.hpp"
#include "msgui/Input.hpp"
#include "msgui/layoutEngine/ITextLayoutEngine.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/renderer/TextRenderer.hpp"

namespace msgui
{
using namespace layoutengine;

class Application;

/* UI Window content manager */
class WindowFrame
{
/*  Since scrollbars and dropdowns need to be displayed on top of the container they attach to and kinda on top of
    everything else, a custom Z start is needed. Same for floating windows or dropdowns.
*/
static constexpr uint32_t SCROLL_LAYER_START = 150;
static constexpr uint32_t DROPDOWN_LAYER_START = 200;
static constexpr uint32_t FLOATING_LAYER_START = 300;

static constexpr uint32_t MAX_DEFAULT_CURSORS = 6;

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
    void resolveOnMouseWheelFromInput(const int32_t x, const int32_t y);
    void resolveOnMouseEnterExitFromInput(const bool entered);
    void resolveOnWindowResizeFromInput(const int32_t newWidth, const int32_t newHeight);

private:
    Logger log_;
    Window window_;
    Input input_;
    FrameStatePtr frameState_{nullptr};
    bool shouldWindowClose_{false};
    ILayoutEnginePtr layoutEngine_{nullptr};
    renderer::TextRenderer textRenderer_;
    ITextLayoutEnginePtr textLayoutEngine_{nullptr};
    std::vector<AbstractNodePtr> allFrameChildNodes_;
    BoxPtr frameBox_{nullptr};
    bool isPrimary_{false};


    static std::array<GLFWcursor*, MAX_DEFAULT_CURSORS> standardCursors_;
    static bool initCursors;
};
using WindowFramePtr = std::shared_ptr<WindowFrame>;
using WindowFrameWPtr = std::weak_ptr<WindowFrame>;
} // msgui