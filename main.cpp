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

    FrameUPtr& frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);

    BoxPtr leftBox = std::make_shared<Box>("BoxLeft");
    BoxPtr middleBox = std::make_shared<Box>("BoxMiddle");
    BoxPtr rightBox = std::make_shared<Box>("BoxRight");

    leftBox->setColor(Utils::hexToVec4("#ffaaffff"));
    middleBox->setColor(Utils::hexToVec4("#aaffffff"));
    rightBox->setColor(Utils::hexToVec4("#ffffaaff"));
    // ButtonPtr btn = std::make_shared<Button>("Btn1");
    // ButtonPtr btn2 = std::make_shared<Button>("Btn2");
    // btn->getTransform().setScale({50, 50, 1});
    // // btn->getTransform().setPos({100, 100, 2});

    // btn2->getTransform().setScale({100, 200, 1});
    // // btn2->getTransform().setPos({170, 100, 2});

    leftBox->getTransform().setScale({200, 200, 1});
    middleBox->getTransform().setScale({200, 200, 1});
    rightBox->getTransform().setScale({200, 200, 1});

    // frame->append(btn);
    // frame->append(box1);
    // frame2->append(btn2);
    // btn2->append(btn3);
    frame->getRoot()->appendMany({leftBox, middleBox, rightBox});
    // box2->append(btn);
    // frame->append(btn2);

    Logger mainLog{"MainLog"};
    // FramePtr frame2;
    // bool frame2;
    // btn->setMouseClickListener([&mainLog, &app]()
    // {
    //     mainLog.infoLn("clicked me");
    //     FrameUPtr& frame2 = app.createFrame("WindowAnnex", WINDOW_W/2, WINDOW_H/2);
    //     uint32_t frame2Id = frame2->getRoot()->getId();
    //     ButtonPtr b = std::make_shared<Button>("BtnSecond");
    //     b->getTransform().setScale({100, 200, 1});

    //     // FrameUPtr* frame2_ref = app.getFrameId(frame2Id+1);
    //     // if (frame2_ref)
    //     // {
    //     //     ButtonPtr b2 = std::make_shared<Button>("BtnThird");
    //     //     b2->getTransform().setScale({200, 100, 1});
    //     //     frame2->getRoot()->append(b2);
    //     // }

    //     frame2->getRoot()->append(b);
    // });

    app.run();

    return 0;
}