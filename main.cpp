#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/BoxDivider.hpp"
#include "core/node/Button.hpp"
#include "core/node/Image.hpp"
#include "core/node/RecycleList.hpp"
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
    mainLog.debugLn("%lu", __cplusplus);
    // Debug& dbg = Debug::get();

    // WindowFramePtr& frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H);
    // frame->getRoot()->getLayout().setType(Layout::Type::GRID)
    //     .setAllowOverflow({true, true})
    //     // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
    //     .setAlignChild({Layout::Align::LEFT, Layout::Align::CENTER})
    //     .setPadding({0});
    // frame->getRoot()->getLayout().setGridDistrib({
    //     .rows = Layout::DistribVec{
    //         Layout::GridDistrib{Layout::GridDistrib::Type::FRAC, 1},
    //         Layout::GridDistrib{Layout::GridDistrib::Type::FRAC, 1},
    //         // Layout::Distrib{Layout::Distrib::Type::ABS, 300}
    //         },
    //     .cols = Layout::DistribVec{
    //         Layout::GridDistrib{Layout::GridDistrib::Type::FRAC, 1},
    //         // Layout::Distrib{Layout::Distrib::Type::FRAC, 1},
    //         // Layout::Distrib{Layout::Distrib::Type::FRAC, 1},
    //         Layout::GridDistrib{Layout::GridDistrib::Type::ABS, 250},
    //         Layout::GridDistrib{Layout::GridDistrib::Type::FRAC, 1}},
    // });

    // AbstractNodePVec nodes;
    // for (int32_t i = 0; i < 2; i++)
    // {
    //     for (int32_t j = 0; j < 3; j++)
    //     {
    //         // Layout::Align align{Layout::Align::BOTTOM_RIGHT};

    //         // BoxPtr ref = std::make_shared<Box>("Boxy");

    //         if (i == 0 && j == 0)
    //         {
    //             ImagePtr img = Utils::make<Image>("Img");
    //             img->getLayout()
    //                 .setScaleType(Layout::ScaleType::REL)
    //                 .setScale({1.0f, 1.0f})
    //                 .setGridStartRC({i, j})
    //                 .setMargin({10, 10, 10, 10})
    //                 ;
    //             img->setImage("assets/textures/container.jpg");

    //             nodes.emplace_back(img);


    //         }
    //         else
    //         {
    //             BoxPtr ref = Utils::make<Box>("Boxy");
    //             ref->setBorderColor(Utils::hexToVec4("#7e0202ff"));
    //             ref->getLayout()
    //                 .setBorder({10, 10, 10, 10})
    //                 // .setBorderRadius({10, 10, 10, 10})
    //                 // .setAlignSelf(align)
    //                 .setScaleType(Layout::ScaleType::REL)
    //                 // .setScale({100, 100})
    //                 .setScale({1.0f, 1.0f})
    //                 .setGridStartRC({i, j})
    //                 .setMargin({10, 10, 10, 10})
    //                 ;

    //             // ref->getLayout().setScale({200, 100 * Utils::random01() + 20});
    //             // ref->getLayout().setAlignSelf(Layout::Align::CENTER);
    //             ref->setColor(Utils::randomRGB());
    //             nodes.emplace_back(ref);

    //         }
    //     }
    // }
    // frame->getRoot()->appendMany(nodes);

    // AbstractNodePtr first = frame->getRoot()->findOneBy([](const auto&) -> bool { return true; });
    // if (first)
    // {
    //     BoxPtr box = Utils::as<Box>(first);
    //     box->setColor(Utils::hexToVec4("#ff0000ff"));

    //     RecycleListPtr list = std::make_shared<RecycleList>("myRec");
    //     list->setColor(Utils::hexToVec4("#ffffffff"));
    //     list->getLayout().setScaleType(Layout::ScaleType::REL).setScale({1.0f, 1.0f});

    //     for (int32_t i = 0; i < 21; i++)
    //     {
    //         list->addItem(Utils::randomRGB());
    //     }

    //     box->append(list);

    WindowFramePtr& frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H);

    std::thread t([mainLog, &app]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        WindowFramePtr& frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H);
    });

    app.setPollMode(Application::PollMode::ON_EVENT);
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setVSync(true);
    app.run();
    t.join();
    return 0;
}