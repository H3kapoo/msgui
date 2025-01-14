#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/BoxDivider.hpp"
#include "core/node/Button.hpp"
#include "core/node/Slider.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/node/utils/LayoutData.hpp"
#include <chrono>
#include <thread>

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
    // frame->getRoot()->props.layout.type = Layout::Type::GRID;
    // frame->getRoot()->props.layout.distribution = {1fr, 10px, 1fr};  // |   eq_space  |10px|  eq_space   |
    // child->props.layout.gridStartX/Y = 0;
    // child->props.layout.gridSpanX/Y = 1;

    // child->props.layout.scaleType.x/y = {Layout::ScaleType::ABSOLUTE/RELATIVE};
    // child->props.layout.scale = {100, 0.5f};
    // child->props.layout.scale = {100, Layout::Calc(0.5f, -100px)};

    frame->getRoot()->props.layout.type = Layout::Type::HORIZONTAL;
    frame->getRoot()->props.layout.allowOverflowX = true;
    frame->getRoot()->props.layout.allowOverflowY = true;
    // frame->getRoot()->props.layout.allowWrap = true;
    // frame->getRoot()->props.layout.border = Layout::TBLR{40, 10, 20, 50};
    frame->getRoot()->props.borderColor = Utils::hexToVec4("#aabb11ff");
    frame->getRoot()->listeners.setOnMouseButtonLeftClick([&]()
    {
        mainLog.debugLn("clicked to 40");
        frame->getRoot()->props.scrollBarSize = 40;
    });

    BoxDividerPtr divider = std::make_shared<BoxDivider>("BoxDivider1");
    divider->props.color = Utils::hexToVec4("#a7b430ff");
    divider->props.layout.type = Layout::Type::HORIZONTAL;
    divider->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};
    divider->props.layout.scale = {1.0f, 1.0f};

    BoxPtr box1 = std::make_shared<Box>("Box1");
    box1->props.color = Utils::hexToVec4("#163f52ff");
    box1->props.layout.type = Layout::Type::HORIZONTAL;
    box1->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};

    BoxPtr box2 = std::make_shared<Box>("Box2");
    box2->props.color = Utils::hexToVec4("#165239ff");
    box2->props.layout.type = Layout::Type::HORIZONTAL;
    box2->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};

    BoxPtr box3 = std::make_shared<Box>("Box3");
    box3->props.color = Utils::hexToVec4("#70ddb0ff");
    box3->props.layout.type = Layout::Type::HORIZONTAL;
    // box3->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};
    box3->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};
    box1->props.layout.scale = {1.0f / 3.0f, 1.0f};
    box2->props.layout.scale = {1.0f / 3.0f, 1.0f};
    box3->props.layout.scale = {1.0f / 3.0f, 1.0f};

    // box1 0.33 min 0.1
    // box2 0.33 min 0.5 ( 0.17 overflow )
    // box3 0.33 min 0.2 0 overflow
    // box1->props.layout.minScale.value.x = 0.4f;
    box1->props.layout.minScale.value.x = 200;
    box2->props.layout.minScale.value.x = 50;
    box3->props.layout.minScale.value.x = 200;
    // box1->props.layout.minScale.value.x = 200;
    // box1->props.layout.minScale.value.x = 200;
    // box2->props.layout.minScale.value.x = 0;
    // box3->props.layout.minScale.value.x = 200;
    // box2->props.layout.minScale.value.x = 200;
    // box2->props.layout.minScale.value.x = 0.1;
    // box3->props.layout.minScale.value.x = 0.1;

    // boxMid2->props.layout.margin = Layout::TBLR{0, 0, 1, 1};
    // boxMid->props.layout.margin = Layout::TBLR{0, 0, 1, 1};

    // divider->appendBoxContainer(box1);
    // box2->props.layout.scale = {1.0f / 2.0f, 1.0f};
    // box1->props.layout.scale = {1.0f / 2.0f, 1.0f};
    // box2->props.layout.scale = {1.0f / 2.0f, 1.0f};
    divider->appendBoxContainers({box1, box2, box3});
    // divider->appendMany({box1, box2});
    // divider->appendMany({box1, boxMid, box2});
    // divider->appendMany({box1, boxMid, box2, boxMid2, box3});
    // theBox->listeners.setOnMouseButtonLeftClick([&]()
    // {
    //     mainLog.debugLn("clicked to 40");
    //     theBox->props.scrollBarSize = 40;
    // });

    ButtonPtr preButton = std::make_shared<Button>("PreButton");
    preButton->getTransform().scale = {200, 50, 1};
    preButton->props.texture = "assets/textures/container.jpg";
    preButton->props.layout.alignSelf = Layout::Align::CENTER;

    // frame->getRoot()->props.layout.alignChildY = Layout::Align::CENTER;
    // frame->getRoot()->props.layout.padding = Layout::TBLR{5};
    // frame->getRoot()->props.layout.spacing = Layout::Spacing::EVEN_WITH_START_GAP;

    AbstractNodePVec nodes;
    int32_t objCnt = 0;
    // for (int32_t i = 0; i < 20'000; i++)
    for (int32_t i = 0; i < objCnt; i++)
    {
        auto& node = nodes.emplace_back(std::make_shared<Button>("Button_Id_" + std::to_string(i)));
        Button* bx = static_cast<Button*>(node.get());
        // bx->props.color = Utils::hexToVec4("#ddaabbff");
        // bx->props.texture = "assets/textures/container.jpg";
        bx->props.layout.alignSelf
            = Layout::Align::TOP;
        // node->setShader(ShaderLoader::load("assets/shader/sdfTest.glsl"));

        int32_t randomX = std::max(150.0f, Utils::random01() * 350);
        int32_t randomY = std::max(150.0f, Utils::random01() * 250);

        // bx->getTransform().scale = {randomX, randomY, 1};
        bx->props.layout.scaleType = {Layout::ScaleType::ABS, Layout::ScaleType::ABS};
        // bx->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};
        // bx->props.layout.scale = {200, 0.5f};
        bx->props.layout.scale = {200, 50};
        // bx->props.layout.scale = {1.0f / objCnt, 200};
        // bx->props.layout.scale = {1.0f / objCnt, 0.5f};
        bx->props.layout.margin = Layout::TBLR{5};
    }
    // theBox->appendMany(nodes);
    // frame->getRoot()->appendMany(nodes);
    // frame->getRoot()->append(preButton);
    frame->getRoot()->append(divider);
    // frame->getRoot()->append(slider);

    frame->getRoot()->printTree();


    // app.setPollMode(Application::PollMode::CONTINUOUS);
    // app.setVSync(Application::Toggle::OFF);
    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(Application::Toggle::ON);
    app.run();
    return 0;
}