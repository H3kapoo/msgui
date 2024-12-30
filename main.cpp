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

    FramePtr frame = std::make_shared<Frame>("Window", WINDOW_W, WINDOW_H);
    // FramePtr frame2 = std::make_shared<Frame>("Window2", WINDOW_W/2, WINDOW_H/2);
    // FramePtr frame3 = std::make_shared<Frame>("Window3", WINDOW_W/2, WINDOW_H/2);
    app.attachFrame(frame);
    // app.attachFrame(frame2);
    // app.attachFrame(frame3);

    BoxPtr box1 = std::make_shared<Box>("Box1");
    BoxPtr box2 = std::make_shared<Box>("Box2");

    ButtonPtr btn = std::make_shared<Button>("Btn1");
    ButtonPtr btn2 = std::make_shared<Button>("Btn2");
    btn->getTransform().setScale({50, 50, 1});
    // btn->getTransform().setPos({100, 100, 2});

    btn2->getTransform().setScale({100, 200, 1});
    // btn2->getTransform().setPos({170, 100, 2});

    box1->getTransform().setScale({200, 200, 1});
    box2->getTransform().setScale({200, 200, 1});

    // frame->append(btn);
    // frame->append(box1);
    // frame2->append(btn2);
    // btn2->append(btn3);
    frame->getRoot()->appendMany({box1, box2, btn});
    // box2->append(btn);
    // frame->append(btn2);

    Logger mainLog{"MainLog"};
    btn->setMouseClickListener([&mainLog, &frame]()
    {
        ButtonPtr b = std::make_shared<Button>("BtnSecond");
        b->getTransform().setScale({100, 200, 1});

        // btn->append(b);
        // box2->append(b);
        frame->getRoot()->append(b);
        // frame->getRoot()->printTree();
    });

    app.run();

    return 0;
}