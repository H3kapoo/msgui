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
    FramePtr frame2 = std::make_shared<Frame>("Window2", WINDOW_W/2, WINDOW_H/2);
    app.attachFrame(frame);
    app.attachFrame(frame2);

    // ButtonPtr btn = std::make_shared<Button>("Btn1");
    // ButtonPtr btn2 = std::make_shared<Button>("Btn2");
    // ButtonPtr btn3 = std::make_shared<Button>("Btn3");
    // btn->getTransform().setScale({100, 100, 2});
    // btn->getTransform().setPos({100, 100, 2});

    // frame->append(btn);
    // btn2->append(btn3);
    // btn->appendMany({btn2, btn3});
    // frame->append(btn2);

    Logger mainLog{"MainLog"};
    // btn->setMouseClickListener([&mainLog, &btn]()
    // {
    //     // ButtonPtr b = std::make_shared<Button>("InnerBtn");
    //     // btn->append(b);
    //     mainLog.infoLn("clicked me button");
    // });

    // frame->printTree();
    app.run();

    return 0;
}