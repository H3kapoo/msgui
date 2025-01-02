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
class Frame
{
public:
    Frame(const std::string& windowName, const uint32_t width, const uint32_t height,
        const bool isPrimary = false);

    // Normal
    AbstractNodePtr getRoot();

    // Getters
    bool isPrimary() const;

private: // friend
    friend Application;

    // Normal
    bool run();

private:
    // Normal
    void renderLayout();
    void updateLayout();
    void resolveNodeRelations();

    // Window Input Resolvers
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
// using FrameUPtr = std::unique_ptr<Frame>;
using FramePtr = std::shared_ptr<Frame>;
} // msgui