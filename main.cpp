#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "core/Application.hpp"
#include "core/Debug.hpp"
// #include "core/Logger.hpp"
// #include "core/Renderer.hpp"
// #include "core/ShaderLoader.hpp"
// #include "core/Window.hpp"
// #include "core/Input.hpp"
// #include "core/node/AbstractNode.hpp"
// #include "core/node/Button.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/Window.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/Button.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/node/utils/LayoutData.hpp"

using namespace msgui;

constexpr uint32_t WINDOW_H = 720;
constexpr uint32_t WINDOW_W = 1280;

int main()
{
    Application& app = Application::get();
    if (!app.init())
    {
        return -1;
    }

    Logger mainLog{"MainLog"};
    Debug& dbg = Debug::get();

    WindowFramePtr frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);

    // frame->getRoot()->enableVScroll();
    frame->getRoot()->props.layout.allowOverflowX = true;
    // frame->getRoot()->props.layout.allowOverflowY = true;

    BoxPtr leftBox = std::make_shared<Box>("BoxLeft");
    BoxPtr middleBox = std::make_shared<Box>("BoxMiddle");
    ButtonPtr middleButton = std::make_shared<Button>("ButtonMiddle");
    BoxPtr rightBox = std::make_shared<Box>("BoxRight");

    leftBox->props.color = Utils::hexToVec4("#ffaaffff");
    middleBox->props.color = Utils::hexToVec4("#aaffffff");
    middleButton->props.color = Utils::hexToVec4("#ddffffff");
    rightBox->props.color = Utils::hexToVec4("#ffffaaff");

    // middleButton->props.texture = "assets/textures/wall.jpg";

    leftBox->getTransform().setScale({200, 200, 1});
    middleBox->getTransform().setScale({200, 200, 1});
    middleButton->getTransform().setScale({200, 200, 1});
    rightBox->getTransform().setScale({200, 200, 1});

    frame->getRoot()->appendMany({leftBox, middleBox, middleButton, rightBox});

    uint32_t frameId{0};
    leftBox->listeners.setOnMouseButton([&mainLog, &app, &frameId](auto btn, auto action, auto x, auto y)
    {
        if (action != 0) { return; }
        mainLog.infoLn("clicked me %d %d %d %d", btn, action, x, y);
    });

    rightBox->listeners.setOnMouseButtonLeftClick([&mainLog, &app, &frame, &frameId, &rightBox]()
    {
        // frame->getRoot()->props.layout.allowOverflowX = frameId % 2? true : false;
        // frame->getRoot()->props.layout.allowOverflowY = frameId % 2? true : false;
        frame->getRoot()->props.layout.orientation =
            frameId % 2 ? Layout::Orientation::HORIZONTAL : Layout::Orientation::VERTICAL;
        
        rightBox->props.color = Utils::randomRGB();
        frameId++;
        // frame->getRoot()->printTree();
    });

    middleButton->listeners.setOnMouseButtonLeftClick([&mainLog, &middleButton, &frameId]()
    {
        middleButton->props.texture = frameId % 2 ? "assets/textures/wall.jpg" : "assets/textures/awesomeface.png";
        frameId++;
        mainLog.debugLn("clicked ME middleBox");
    });

    // middleButton->listeners.setOnMouseButtonLeftClick([&mainLog]()
    // {
    //     mainLog.debugLn("clicked ME middleBox 2");
    // });
    // msgui::Window::disableVSync();
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.run();

    return 0;
}