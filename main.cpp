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

    // WindowFramePtr frame  = 
    app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H, true);
    // WindowFramePtr frame2 = 
    app.createFrame("SecPrimary", WINDOW_W/2, WINDOW_H/2);
    // frame->getRoot()->props.layout.type = Layout::Type::GRID;
    // frame->getRoot()->props.layout.distribution = {1fr, 10px, 1fr};  // |   eq_space  |10px|  eq_space   |
    // child->props.layout.gridStartX/Y = 0;
    // child->props.layout.gridSpanX/Y = 1;

    // child->props.layout.scaleType.x/y = {Layout::ScaleType::ABSOLUTE/RELATIVE};
    // child->props.layout.scale = {100, 0.5f};
    // child->props.layout.scale = {100, Layout::Calc(0.5f, -100px)};

    // frame->getRoot()->setColor(Utils::randomRGB());
    // frame2->getRoot()->setColor(Utils::randomRGB());

        // .setPadding({10, 10, 10, 10});
    // frame->getRoot()->props.borderColor = Utils::hexToVec4("#aabb11ff");
    // frame->getRoot()->props.color = Utils::hexToVec4("#aabb11ff");

    // RecycleListPtr recycleList = std::make_shared<RecycleList>("RecycleList1");

    // recycleList->setColor(Utils::hexToVec4("#1185bbff"));
    // recycleList->getLayout()
    //     .setMargin({5, 5, 5, 5})
    //     .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
    //     .setScale({400, 0.8f});
    //     // .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::ABS})
    //     // .setScale({400, 500});

    // for (int32_t i = 0; i < 10; i++)
    // {
    //     recycleList->addItem(Utils::randomRGB());
    // }

    // BoxDividerPtr divider = std::make_shared<BoxDivider>("BoxDivider1");
    // divider->props.color = Utils::hexToVec4("#a7b430ff");
    // divider->getLayout()
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

    // box1->getLayout().setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});
    // box2->getLayout().setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});
    // box3->getLayout().setMinScale({100, 100}); //.setMargin({10, 10, 10, 10});

    // box2->append(recycleList);
    // frame->getRoot()->listeners.setOnMouseButton([&](int32_t btn, int32_t action, int32_t, int32_t)
    // {
    //     if (action == GLFW_RELEASE && btn == GLFW_MOUSE_BUTTON_LEFT)
    //     {
    //         recycleList->addItem(Utils::randomRGB());
    //         recycleList->addItem(Utils::randomRGB());
    //         recycleList->addItem(Utils::randomRGB());
    //     }
    //     if (action == GLFW_RELEASE && btn == GLFW_MOUSE_BUTTON_RIGHT)
    //     {
    //         recycleList->removeTailItems(1);
    //     }

    //     // mainLog.debugLn("clicked");
    // });
    // ButtonPtr preButton = std::make_shared<Button>("PreButton");
    // preButton->getTransform().scale = {200, 50, 1};
    // preButton->props.texture = "assets/textures/container.jpg";
    // preButton->getLayout().alignSelf = Layout::Align::CENTER;

    // frame->getRoot()->props.layout.alignChildY = Layout::Align::CENTER;
    // frame->getRoot()->props.layout.padding = Layout::TBLR{5};
    // frame->getRoot()->props.layout.spacing = Layout::Spacing::EVEN_WITH_START_GAP;

    // AbstractNodePVec nodes;
    // for (int32_t i = 0; i < 4; i++)
    // {
    //     BoxPtr ref = std::make_shared<Box>("Boxy");
    //     ref->getLayout().setScale({200, 100 * Utils::random01() + 20});
    //     // ref->getLayout().setAlignSelf(Layout::Align::CENTER);
    //     ref->setColor(Utils::randomRGB());
    //     nodes.emplace_back(ref);
    // }
    // // theBox->appendMany(nodes);
    // frame->getRoot()->appendMany(nodes);
    // frame->getRoot()->append(divider);
    // frame->getRoot()->append(recycleList);

    // frame->getRoot()->printTree();

    // app.setVSync(Application::Toggle::OFF);
    app.setPollMode(Application::PollMode::ON_EVENT);
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setVSync(Application::Toggle::ON);
    app.run();
    return 0;
}