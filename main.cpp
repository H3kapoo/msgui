#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
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
    frame->getRoot()->props.layout.border = Layout::TBLR{40, 10, 20, 50};
    frame->getRoot()->props.borderColor = Utils::hexToVec4("#aabb11ff");
    frame->getRoot()->listeners.setOnMouseButtonLeftClick([&]()
    {
        mainLog.debugLn("clicked to 40");
        frame->getRoot()->props.scrollBarSize = 40;
    });

    BoxPtr theBox = std::make_shared<Box>("theBox");
    theBox->props.color = Utils::hexToVec4("#ffbbffff");
    theBox->props.layout.allowOverflowX = true;
    theBox->props.layout.allowOverflowY = true;
    theBox->props.layout.type = Layout::Type::VERTICAL;
    theBox->props.layout.border = Layout::TBLR{40, 10, 20, 50};
    theBox->props.borderColor = Utils::hexToVec4("#aaff11ff");
    theBox->getTransform().scale = {500, 400, 1};
    theBox->listeners.setOnMouseButtonLeftClick([&]()
    {
        mainLog.debugLn("clicked to 40");
        theBox->props.scrollBarSize = 40;
    });

    ButtonPtr preButton = std::make_shared<Button>("PreButton");
    preButton->getTransform().scale = {200, 50, 1};
    preButton->props.texture = "assets/textures/container.jpg";
    preButton->props.layout.alignSelf = Layout::Align::CENTER;

    // SliderPtr slider = std::make_shared<Slider>("SliderPost");
    // slider->props.orientType = Layout::Type::VERTICAL;
    // slider->getTransform().scale = {30, 300, 1};
    // slider->props.color = Utils::hexToVec4("#eeffaaff");
    // slider->props.layout.borderRadius = Layout::TBLR{5};
    // slider->getKnobRef()->props.layout.borderRadius = Layout::TBLR{5};
    // slider->getKnobRef()->getTransform().scale = {30, 30, 1};
    // slider->props.slideFrom = 0;
    // slider->props.slideTo = 1;
    // // slider->props.slideValue = 30;
    // slider->listeners.setOnSlideValueChanged([&mainLog, &preButton](float newValue)
    // {
    //     preButton->props.color.g = newValue;
    // });

    frame->getRoot()->props.layout.padding = Layout::TBLR{5};
    frame->getRoot()->props.layout.spacing = Layout::Spacing::EVEN_WITH_START_GAP;

    AbstractNodePVec nodes;
    int32_t objCnt = 3;
    // for (int32_t i = 0; i < 20'000; i++)
    for (int32_t i = 0; i < objCnt; i++)
    {
        auto& node = nodes.emplace_back(std::make_shared<Button>("Button_Id_" + std::to_string(i)));
        Button* bx = static_cast<Button*>(node.get());
        // bx->props.color = Utils::hexToVec4("#ddaabbff");
        bx->props.texture = "assets/textures/container.jpg";
        bx->props.layout.alignSelf
            = Layout::Align::TOP;
        // node->setShader(ShaderLoader::load("assets/shader/sdfTest.glsl"));

        int32_t randomX = std::max(150.0f, Utils::random01() * 350);
        int32_t randomY = std::max(150.0f, Utils::random01() * 250);

        // bx->getTransform().scale = {randomX, randomY, 1};
        // bx->props.layout.scaleType = {Layout::ScaleType::ABS, Layout::ScaleType::ABS};
        bx->props.layout.scaleType = {Layout::ScaleType::REL, Layout::ScaleType::REL};
        // bx->props.layout.scale = {200, 0.5f};
        // bx->props.layout.scale = {200, 200};
        bx->props.layout.scale = {1.0f / objCnt, 200};
        bx->props.layout.scale = {1.0f / objCnt, 1.0f};
        bx->props.layout.margin = Layout::TBLR{5};
    }
    // theBox->appendMany(nodes);
    frame->getRoot()->appendMany(nodes);
    // frame->getRoot()->append(preButton);
    // frame->getRoot()->append(theBox);
    // frame->getRoot()->append(slider);

    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(Application::Toggle::ON);
    app.run();
    return 0;
}