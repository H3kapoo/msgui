#pragma once

#include <memory>

#include "core/layoutEngine/ILayoutEngine.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
// Friend
class Application;

/* UI Window content manager */
class WindowFrame
{
static constexpr uint32_t SCROLL_LAYER_START = 250;

public:
    WindowFrame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);

    BoxPtr getRoot();

    // Getters
    bool isPrimary() const;

private: // friend
    friend Application;

    bool run();

private:
    void renderLayout();
    void updateLayout();
    void resolveNodeRelations();

    void resolveOnMouseButtonFromInput(int32_t btn, int32_t action);
    void resolveOnMouseMoveFromInput(int32_t x, int32_t y);

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
};
using WindowFramePtr = std::shared_ptr<WindowFrame>;
} // msgui