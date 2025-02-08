#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/Window.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/BoxDivider.hpp"
#include "core/node/Button.hpp"
#include "core/node/Dropdown.hpp"
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

    AbstractNodePVec nodes;

    DropdownPtr bx = Utils::make<Dropdown>("Drop");
    bx->setColor(Utils::hexToVec4("#ffaa00ff"));
    bx->getLayout()
        .setScaleType(Layout::ScaleType::ABS)
        .setScale({200, 200})
        .setMargin({10, 10, 60, 10});
    nodes.emplace_back(bx);

    for (int32_t i = 0; i < 3; i++)
    {
        ButtonPtr butt = Utils::make<Button>("Btn");
        butt->setColor(Utils::randomRGB());
        butt->getLayout()
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setScale({1.0f, 20})
            .setMargin({1, 1, 0, 0})
            ;
        bx->addMenuItem(butt);
    }

    DropdownPtr bx2 = Utils::make<Dropdown>("Drop");
    bx2->setColor(Utils::hexToVec4("#ffaa00ff"));
    bx2->getLayout()
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        .setScale({1.0f, 20})
        .setMargin({1, 1, 0, 0});
    bx->addMenuItem(bx2);

    for (int32_t i = 0; i < 6; i++)
    {
        ButtonPtr butt = Utils::make<Button>("Btn");
        butt->setColor(Utils::randomRGB());
        butt->getLayout()
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setScale({1.0f, 20})
            .setMargin({1, 1, 0, 0})
            ;
        bx2->addMenuItem(butt);
    }
    /*
        -- DropDown (custom node type that will position holding box as it wants)
            -- Box (hovering wherever we put it, on Z over everything else)
                -- Item (button maybe)
                -- Item (button maybe)
        
        Nesting (submenus) should be as easy add appending a new DropDown node with .append
        Just for testing purposes the dropdown will appear exactly right side of the parent.
        The Box of the dropdown will IGNORE the viewable area of it's parent
        The Box will be placed at the highest Z available

        After clicking the dropdown, the Box will stay visible as long as the user doesnt click
        away from it. If user clicks on an item inside the dropdown then it will stay on if that
        item happens to be a sub-dropdown.
        Concept of focus? Nah

        API:
        dd.append/remove(ButtonNode/or whatever);


    */
    // ImagePtr bx = Utils::make<Image>("Btn");
    // bx->setColor(Utils::randomRGB());
    // bx->getLayout()
    //     .setScaleType(Layout::ScaleType::ABS)
    //     .setScale({100, 100})
    //     .setMargin({10, 10, 10, 10})
    //     ;
    // bx->setImage("assets/textures/container.jpg");
    // // bx->setTint(Utils::hexToVec4("#a809a8ff"));
    // nodes.emplace_back(bx);
    frame->getRoot()->appendMany(nodes);
    // AbstractNodePtr first = frame->getRoot()->findOneBy([](const auto&) -> bool { return true; });

    // std::thread t([mainLog, &app]()
    // {
    //     // std::this_thread::sleep_for(std::chrono::seconds(2));
    //     // img->setImage("assets/textures/wall.jpg");
    //     // img->setImage("assets/textures/awesomeface.png");

    //     WindowFramePtr& fr = app.createFrame("WindowPrimary2", WINDOW_W, WINDOW_H);
    //     fr->getRoot()->getLayout().setType(Layout::Type::HORIZONTAL)
    //         .setAllowOverflow({true, true})
    //         // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
    //         .setAlignChild({Layout::Align::LEFT, Layout::Align::CENTER})
    //         .setPadding({0});

    //     mainLog.debugLn("id is %u", fr->getRoot()->getId());

    //     AbstractNodePVec nodes;
    //     // ImagePtr img = Utils::make<Image>("Img");
    //     // img->getLayout()
    //     //     .setScaleType(Layout::ScaleType::REL)
    //     //     .setScale({1.0f, 1.0f})
    //     //     .setMargin({10, 10, 10, 10})
    //     //     ;
    //     // img->setImage("assets/textures/container.jpg");

    //     // nodes.emplace_back(img);
    //     // fr->getRoot()->appendMany(nodes);
    // });

    // std::thread t2([mainLog, frame]()
    // {   
    //     Application& ap = Application::get();
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     ImagePtr img = Utils::make<Image>("Img");
    //     img->getLayout()
    //         .setScaleType(Layout::ScaleType::REL)
    //         .setScale({1.0f, 1.0f})
    //         .setMargin({10, 10, 10, 10})
    //         ;
    //     img->setImage("assets/textures/wall.jpg");
    //     ap.getFrameId(3)->getRoot()->append(img);
    //     // Utils::as<Image>(first)->setImage("assets/textures/wall.jpg");
    // });

    app.setPollMode(Application::PollMode::ON_EVENT);
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    app.setVSync(true);
    app.run();
    // t.join();
    // t2.join();
    return 0;
}