#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/BoxDivider.hpp"
#include "core/node/Button.hpp"
#include "core/node/RecycleList.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/node/utils/LayoutData.hpp"
#include <GLFW/glfw3.h>

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


    frame->getRoot()->props.layout
        .setType(Layout::Type::HORIZONTAL)
        .setAllowOverflow({true, true})
        .setAllowWrap(true);
        // .setPadding({10, 10, 10, 10});
    frame->getRoot()->props.borderColor = Utils::hexToVec4("#aabb11ff");

    RecycleListPtr recycleList = std::make_shared<RecycleList>("RecycleList1");
    
    recycleList->props.color = Utils::hexToVec4("#1185bbff");
    recycleList->props.layout
        .setMargin({100, 0, 50, 0})
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({400, 0.8f});
        // .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::ABS})
        // .setScale({400, 500});

    frame->getRoot()->listeners.setOnMouseButton([&](int32_t btn, int32_t action, int32_t, int32_t)
    {
        if (action == GLFW_RELEASE && btn == GLFW_MOUSE_BUTTON_LEFT)
        {
            recycleList->addItem(Utils::randomRGB());
            recycleList->addItem(Utils::randomRGB());
            recycleList->addItem(Utils::randomRGB());
        }
        if (action == GLFW_RELEASE && btn == GLFW_MOUSE_BUTTON_RIGHT)
        {
            recycleList->removeTailItems(1);
        }

        mainLog.debugLn("clicked");
    });
    // BoxDividerPtr divider = std::make_shared<BoxDivider>("BoxDivider1");
    // divider->props.color = Utils::hexToVec4("#a7b430ff");
    // divider->props.layout
    //     .setType(Layout::Type::HORIZONTAL)
    //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
    //     .setScale({1.0f, 1.0f})
    //     .setPadding({10, 10, 10, 10});

    // divider->createSlots(3, {1.0f / 3, 1.0f / 3, 1.0f / 3});
    // BoxPtr box1 = divider->getSlot(0);
    // BoxPtr box2 = divider->getSlot(1);
    // BoxPtr box3 = divider->getSlot(2);

    // box1->props.color = Utils::hexToVec4("#163f52ff");
    // box2->props.color = Utils::hexToVec4("#165239ff");
    // box3->props.color = Utils::hexToVec4("#70ddb0ff");

    // box1->props.layout.setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});
    // box2->props.layout.setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});
    // box3->props.layout.setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});

    // box2->props.layout.setAllowOverflow({true, false});

    // ButtonPtr btn1 = std::make_shared<Button>("Button1");
    // btn1->props.layout.setScale({300, 50});
    // btn1->props.color = Utils::hexToVec4("#c2b509ff");

    // box2->append(btn1);

    // BoxDividerPtr divider2 = std::make_shared<BoxDivider>("BoxDivider2");
    // divider2->props.color = Utils::hexToVec4("#393a2eff");
    // divider2->props.layout
    //     .setType(Layout::Type::VERTICAL)
    //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
    //     .setScale({1.0f, 1.0f});

    // divider2->createSlots(2, {1.0f / 2, 1.0f / 2});


    // BoxPtr box21 = divider2->getSlot(0);
    // BoxPtr box22 = divider2->getSlot(1);

    // box21->props.color = Utils::hexToVec4("#911572ff");
    // box22->props.color = Utils::hexToVec4("#180a97ff");
    // box21->props.layout.setMinScale({0, 100});
    // box22->props.layout.setMinScale({0, 100});

    // box1->append(divider2);

    // BoxDividerPtr divider3 = std::make_shared<BoxDivider>("BoxDivider3");
    // divider3->props.color = Utils::hexToVec4("#393a2eff");
    // divider3->props.layout
    //     .setType(Layout::Type::HORIZONTAL)
    //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
    //     .setScale({1.0f, 1.0f});

    // divider3->createSlots(2, {0.6f, 0.4f});
    // BoxPtr box31 = divider3->getSlot(0);
    // BoxPtr box32 = divider3->getSlot(1);

    // box31->props.color = Utils::hexToVec4("#911572ff");
    // box32->props.color = Utils::hexToVec4("#180a97ff");
    // // box31->props.layout.setMinScale({0, 100});
    // // box32->props.layout.setMinScale({0, 100});
    // box31->props.layout.setMinScale({100, 0});
    // box32->props.layout.setMinScale({100, 0});


    // box1->append(divider2);
    // box3->append(divider3);
    // bool mybool{false};
    // box1->listeners.setOnMouseButtonLeftClick([&divider, &mybool]()
    // {
    //     mybool = !mybool;
    //     if (mybool)
    //     {
    //         divider->props.layout.setType(Layout::Type::VERTICAL);
    //     }
    //     else
    //     {
    //         divider->props.layout.setType(Layout::Type::HORIZONTAL);
    //     }
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
    // frame->getRoot()->append(divider);
    frame->getRoot()->append(recycleList);
    // frame->getRoot()->append(slider);

    // frame->getRoot()->printTree();


    // app.setVSync(Application::Toggle::OFF);
    app.setPollMode(Application::PollMode::ON_EVENT);
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setVSync(Application::Toggle::ON);
    app.run();
    return 0;
}