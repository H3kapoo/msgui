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
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/Button.hpp"
#include "core/node/Frame.hpp"

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

    Debug& dbg = Debug::get();

    FramePtr frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);

    BoxPtr leftBox = std::make_shared<Box>("BoxLeft");
    BoxPtr middleBox = std::make_shared<Box>("BoxMiddle");
    BoxPtr middleBox2 = std::make_shared<Box>("BoxMiddle2");
    BoxPtr rightBox = std::make_shared<Box>("BoxRight");

    leftBox->setColor(Utils::hexToVec4("#ffaaffff"));
    middleBox->setColor(Utils::hexToVec4("#aaffffff"));
    middleBox2->setColor(Utils::hexToVec4("#ddffffff"));
    rightBox->setColor(Utils::hexToVec4("#ffffaaff"));

    leftBox->getTransform().setScale({200, 200, 1});
    middleBox->getTransform().setScale({200, 200, 1});
    middleBox2->getTransform().setScale({200, 200, 1});
    rightBox->getTransform().setScale({200, 200, 1});

    frame->getRoot()->appendMany({leftBox, middleBox, middleBox2, rightBox});
    // frame->getRoot()->removeMany({"one", "two"});

    Logger mainLog{"MainLog"};

    uint32_t frameId{0};
    leftBox->getListeners().setOnMouseButton([&mainLog, &app, &frameId](auto btn, auto action, auto x, auto y)
    {
        if (action != 0) { return; }

        mainLog.infoLn("clicked me %d %d %d %d", btn, action, x, y);
        FramePtr frame2 = app.createFrame("WindowAnnex", WINDOW_W/2, WINDOW_H/2);
        frameId = frame2->getRoot()->getId();
    });

    rightBox->getListeners().setOnMouseButtonLeftClick([&mainLog, &app, &frame, &frameId]()
    {
        mainLog.infoLn("clicked right");
        if (FramePtr secondFrame = app.getFrameId(frameId))
        {
            mainLog.infoLn("has it");
            AbstractNodePVec element = frame->getRoot()->removeMany({"BoxMiddle", "BoxMiddle3"});
            if (element.size())
            {
                mainLog.infoLn("Got middle box %ld", element.size());
                secondFrame->getRoot()->appendMany(element);
            }
        }
    });

    middleBox->getListeners().setOnMouseButtonLeftClick([&mainLog]()
    {
        mainLog.debugLn("clicked ME middleBox");
    });

    middleBox2->getListeners().setOnMouseButtonLeftClick([&mainLog]()
    {
        mainLog.debugLn("clicked ME middleBox 2");
    });

    app.run();

    return 0;
}