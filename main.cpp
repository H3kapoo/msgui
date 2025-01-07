#include "core/Application.hpp"
#include "core/Debug.hpp"
// #include "core/Logger.hpp"
// #include "core/Renderer.hpp"
#include "core/ShaderLoader.hpp"
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
#include <cstdint>

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
    // Debug& dbg = Debug::get();

    WindowFramePtr frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);
    // frame->getRoot()->props.layout.alignX/Y = Layout::Align::TOP;
    // frame->getRoot()->props.layout.alignX/Y = Layout::Align::CENTER;
    // frame->getRoot()->props.layout.alignX/Y = Layout::Align::BOT;

    // frame->getRoot()->props.layout.margin = Layout::TBLR{10, 5, 10, 5};
    // frame->getRoot()->props.layout.padding = Layout::TBLR{10, 5, 10, 5};
    // frame->getRoot()->props.layout.border = Layout::TBLR{10, 5, 10, 5};
    // frame->getRoot()->props.layout.borderRadius = Layout::TBLR{10, 5, 10, 5};
    // frame->getRoot()->props.layout.borderRadius = Layout::TBLR{10, 5, 10, 5};

    // frame->getRoot()->props.layout.spacing = Layout::Spacing::TIGHT;                    // |***        |
    // frame->getRoot()->props.layout.spacing = Layout::Spacing::EVEN_WITH_NO_START_GAP;   // |*    *    *|
    // frame->getRoot()->props.layout.spacing = Layout::Spacing::EVEN_WITH_START_GAP;      // |  *  *  *  |

    // frame->getRoot()->props.layout.allowOverflowX = true;
    // frame->getRoot()->props.layout.allowOverflowY = true;

    BoxPtr theBox = std::make_shared<Box>("theBox");
    theBox->props.color = Utils::hexToVec4("#ffbbffff");
    theBox->props.layout.allowOverflowX = true;
    theBox->props.layout.allowOverflowY = true;
    theBox->getTransform().scale = {700, 400, 1};
    theBox->setShader(ShaderLoader::load("assets/shader/sdfTest.glsl"));
    // theBox->getShader() = ShaderLoader::load("assets/shader/sdfTest.glsl");

    ButtonPtr preButton = std::make_shared<Button>("PreButton");
    preButton->getTransform().scale = {200, 50, 1};
    preButton->props.texture = "assets/textures/container.jpg";

    AbstractNodePVec nodes;
    // for (int32_t i = 0; i < 60'000; i++)
    for (int32_t i = 0; i < 0; i++)
    {
        auto& node = nodes.emplace_back(std::make_shared<Box>("Button_Id_" + std::to_string(i)));
        // static_cast<Box*>(node.get())->props.texture = "assets/textures/container.jpg";
        static_cast<Box*>(node.get())->props.color = Utils::randomRGB();
        static_cast<Box*>(node.get())->getTransform().scale = {100, 600, 1};

        if (i == 9)
        {
            auto strangeBox = std::make_shared<Box>("StrangeBox");
            strangeBox->props.color = Utils::randomRGB();
            strangeBox->getTransform().scale = {200, 300, 1};
            node->append(strangeBox);
        }
    }
    theBox->appendMany(nodes);
    frame->getRoot()->append(preButton);
    frame->getRoot()->append(theBox);
    frame->getRoot()->listeners.setOnMouseButtonLeftClick([&]()
    {
        ShaderLoader::reload("assets/shader/sdfTest.glsl");
        // theBox->setShader(ShaderLoader::load("assets/shader/sdfTest.glsl"));
    });
    // BoxPtr leftBox = std::make_shared<Box>("BoxLeft");
    // BoxPtr middleBox = std::make_shared<Box>("BoxMiddle");
    // ButtonPtr middleButton = std::make_shared<Button>("ButtonMiddle");
    // BoxPtr rightBox = std::make_shared<Box>("BoxRight");

    // leftBox->props.color = Utils::hexToVec4("#ffaaffff");
    // middleBox->props.color = Utils::hexToVec4("#aaffffff");
    // middleButton->props.color = Utils::hexToVec4("#ddffffff");
    // rightBox->props.color = Utils::hexToVec4("#ffffaaff");

    // leftBox->getTransform().setScale({200, 200, 1});
    // middleBox->getTransform().setScale({200, 200, 1});
    // middleButton->getTransform().setScale({200, 100, 1});
    // rightBox->getTransform().setScale({500, 200, 1});

    // frame->getRoot()->appendMany({leftBox, middleBox, middleButton, rightBox});

    // uint32_t frameId{0};
    // uint32_t anotherId{0};
    // leftBox->listeners.setOnMouseButton([&](auto btn, auto action, auto x, auto y)
    // {
    //     if (action != 0) { return; }
    //     mainLog.infoLn("clicked me %d %d %d %d", btn, action, x, y);
    //     // frame->getRoot()->props.layout.allowWrap = true;
    //     anotherId++;
    // });

    // rightBox->listeners.setOnMouseButtonLeftClick([&]()
    // {
    //     // frame->getRoot()->props.layout.allowOverflowX = frameId % 2? true : false;
    //     // frame->getRoot()->props.layout.allowOverflowY = frameId % 2? true : false;
    //     frame->getRoot()->props.layout.orientation =
    //         frameId % 2 ? Layout::Orientation::HORIZONTAL : Layout::Orientation::VERTICAL;
        
    //     rightBox->props.color = Utils::randomRGB();
    //     frameId++;
    // });

    // middleButton->props.texture = "assets/textures/wall.jpg";
    // middleButton->listeners.setOnMouseButtonLeftClick([&]()
    // {
    //     // ButtonPtr newBtn = std::make_shared<Button>("NewButton");
    //     // newBtn->getTransform().setScale({100, 100, 1});
    //     // newBtn->props.color = Utils::randomRGB();
    //     // frame->getRoot()->append(newBtn);

    //     // newBtn->listeners.setOnMouseButtonLeftClick([&mainLog]()
    //     // {
    //     //     mainLog.infoLn("Fac bani cand fac muzica");
    //     // });

    //     // frame->getRoot()->props.layout.allowOverflowX = frameId % 2 ? true : false;
    //     // frame->getRoot()->props.layout.allowOverflowY = frameId % 2 ? true : false;
    //     // mainLog.debugLn("ALLOW OVERFLOW: %d", frameId % 2 ? true : false);
    //     // frame->getRoot()->props.layout.allowOverflowY = frameId % 2? true : false;
    //     // frameId++;
    // });


    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(Application::Toggle::ON);
    app.run();

    return 0;
}