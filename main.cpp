#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/Button.hpp"
#include "core/node/Slider.hpp"
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
    // Debug& dbg = Debug::get();

    WindowFramePtr frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);
    // frame->getRoot()->props.layout.type = Layout::Type::GRID;
    // frame->getRoot()->props.layout.distribution = {1fr, 10px, 1fr};  // |   eq_space  |10px|  eq_space   |
    // child->props.layout.gridStartX/Y = 0;
    // child->props.layout.gridSpanX/Y = 1;

    frame->getRoot()->props.layout.type = Layout::Type::HORIZONTAL;
    frame->getRoot()->props.layout.allowOverflowX = true;
    frame->getRoot()->props.layout.allowOverflowY = true;
    frame->getRoot()->props.layout.border = Layout::TBLR{40, 10, 20, 50};
    frame->getRoot()->props.borderColor = Utils::hexToVec4("#aabb11ff");
    frame->getRoot()->listeners.setOnMouseButtonLeftClick([&]()
    {
        mainLog.debugLn("clicked to 40");
        frame->getRoot()->props.sbSize = 40;
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
        theBox->props.sbSize = 40;
    });

    ButtonPtr preButton = std::make_shared<Button>("PreButton");
    preButton->getTransform().scale = {200, 50, 1};
    preButton->props.texture = "assets/textures/container.jpg";
    preButton->props.layout.alignSelf = Layout::Align::CENTER;

    SliderPtr slider = std::make_shared<Slider>("SliderPost");
    slider->props.orientation = Slider::Orientation::VERTICAL;
    // slider->getTransform().scale = {300, 50, 1};
    slider->getTransform().scale = {50, 300, 1};
    slider->props.color = Utils::hexToVec4("#eeffaaff");
    slider->props.slideFrom = 0;
    slider->props.slideTo = 100;
    slider->props.slideValue = 30;
    mainLog.debugLn("value set %f", slider->props.slideValue.value);
    // slider->props.layout.alignSelf = Layout::Align::CENTER;

    AbstractNodePVec nodes;
    // for (int32_t i = 0; i < 20'000; i++)
    for (int32_t i = 0; i < 2; i++)
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

        bx->getTransform().scale = {randomX, randomY, 1};
        // bx->getTransform().scale = {100, 100, 1};
        bx->props.color = Utils::randomRGB();

        // if (i == 3)
        // {
        //     auto nn = std::make_shared<Box>("Box_Id_" + std::to_string(i));
        //     auto nnb = std::make_shared<Button>("Btn_Id_" + std::to_string(i));
        //     nn->getTransform().scale = {200, 200, 1};
        //     nn->props.color = Utils::hexToVec4("#ddaabbff");
        //     nn->props.layout.allowOverflowX = true;
        //     nn->props.layout.allowOverflowY = true;
        //     // nn->props.layout.border = Layout::TBLR{10, 10, 10, 10};
        //     nn->props.layout.borderRadius = Layout::TBLR{10, 10, 10, 10};
        //     nn->props.borderColor = Utils::hexToVec4("#00ff00ff");
        //     nodes.emplace_back(nn);

        //     nnb->getTransform().scale = {300, 200, 1};
        //     nnb->props.texture = "assets/textures/wall.jpg";
        //     nn->append(nnb);
        // }
    }
    // theBox->appendMany(nodes);
    frame->getRoot()->appendMany(nodes);
    frame->getRoot()->append(preButton);
    // frame->getRoot()->append(theBox);
    frame->getRoot()->append(slider);

    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(Application::Toggle::ON);
    app.run();

    return 0;
}