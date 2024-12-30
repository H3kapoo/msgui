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
    Frame(const std::string& windowName, const uint32_t width, const uint32_t height);

    // Normal
    AbstractNodePtr getRoot();

private: // friend
    friend Application;

    // Normal
    bool run();

private:
    // Normal
    void renderLayout();
    void updateLayout();
    void resolveNodeRelations();
    void resolveOnMouseButtonFromInput(int32_t btn, int32_t action);

private:
    Logger log_;
    Window window_;
    Input input_;
    FrameStatePtr frameState_{nullptr};
    bool shouldWindowClose_{false};
    ILayoutEnginePtr layoutEngine_{nullptr}; // this should be a singleton maybe as it doesnt depend on anything
    std::vector<AbstractNodePtr> allFrameChildNodes_;
    BoxPtr frameBox_{nullptr};
};
using FramePtr = std::shared_ptr<Frame>;
} // msgui