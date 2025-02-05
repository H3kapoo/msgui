#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/Window.hpp"
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

bool trigger = false;
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

    WindowFramePtr& frame = app.createFrame("WindowPrimary", WINDOW_W, WINDOW_H);
    frame->getRoot()->getLayout().setType(Layout::Type::HORIZONTAL)
        .setAllowOverflow({true, true})
        // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
        .setAlignChild({Layout::Align::LEFT, Layout::Align::CENTER})
        .setPadding({0});

    // AbstractNodePVec nodes;
    // ImagePtr img = Utils::make<Image>("Img");
    // img->getLayout()
    //     .setScaleType(Layout::ScaleType::REL)
    //     .setScale({1.0f, 1.0f})
    //     .setMargin({10, 10, 10, 10})
    //     ;
    // img->setImage("assets/textures/container.jpg");

    // nodes.emplace_back(img);
    // frame->getRoot()->appendMany(nodes);
    // AbstractNodePtr first = frame->getRoot()->findOneBy([](const auto&) -> bool { return true; });

    std::thread t([mainLog, &app]()
    {
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        // img->setImage("assets/textures/wall.jpg");
        // img->setImage("assets/textures/awesomeface.png");

        WindowFramePtr& fr = app.createFrame("WindowPrimary2", WINDOW_W, WINDOW_H);
        fr->getRoot()->getLayout().setType(Layout::Type::HORIZONTAL)
            .setAllowOverflow({true, true})
            // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
            .setAlignChild({Layout::Align::LEFT, Layout::Align::CENTER})
            .setPadding({0});

        mainLog.debugLn("id is %u", fr->getRoot()->getId());

        AbstractNodePVec nodes;
        // ImagePtr img = Utils::make<Image>("Img");
        // img->getLayout()
        //     .setScaleType(Layout::ScaleType::REL)
        //     .setScale({1.0f, 1.0f})
        //     .setMargin({10, 10, 10, 10})
        //     ;
        // img->setImage("assets/textures/container.jpg");

        // nodes.emplace_back(img);
        // fr->getRoot()->appendMany(nodes);
    });

    std::thread t2([mainLog, frame]()
    {   
        Application& ap = Application::get();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        ImagePtr img = Utils::make<Image>("Img");
        img->getLayout()
            .setScaleType(Layout::ScaleType::REL)
            .setScale({1.0f, 1.0f})
            .setMargin({10, 10, 10, 10})
            ;
        img->setImage("assets/textures/container.jpg");
        ap.getFrameId(2)->getRoot()->append(img);
        // Utils::as<Image>(first)->setImage("assets/textures/wall.jpg");
    });

    app.setPollMode(Application::PollMode::ON_EVENT);
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setVSync(true);
    app.run();
    t.join();
    t2.join();
    return 0;
}