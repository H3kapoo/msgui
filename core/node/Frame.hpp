#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Window.hpp"
#include "core/Input.hpp"

#include <GLFW/glfw3.h>
#include <memory>

namespace msgui
{

class Application;
class Frame : public AbstractNode
{
public:
    Frame(const std::string& windowName, const uint32_t width, const uint32_t height);

private: // friend
    friend Application;
    bool run();

private:
    void renderLayout();
    void updateLayout();
    void resolveNodeRelations();
    void resolveOnMouseButtonFromInput(int32_t btn, int32_t action);

    // Overrides
    void setShaderAttributes() override;

private:
    Window window_;
    Input input_;
    bool shouldWindowClose_{false};
    std::vector<AbstractNodePtr> allFrameChildNodes_;
};
using FramePtr = std::shared_ptr<Frame>;
} // msgui