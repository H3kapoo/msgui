#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/Window.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/recyclelist/RecycleList.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/MouseButton.hpp"
#include "msgui/nodeEvent/NodeEventManager.hpp"
#include <chrono>
#include <memory>
#include <thread>

using namespace msgui;
using namespace msgui::nodeevent;

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
    {
        DropdownPtr bx = Utils::make<Dropdown>("Drop");
        bx->setColor(Utils::hexToVec4("#ffaa00ff"));
        bx->getLayout()
            .setScaleType(Layout::ScaleType::ABS)
            .setScale({200, 50})
            .setMargin({10, 10, 10, 200});

        ButtonPtr btn = Utils::make<Button>("mybutton");
        btn->getLayout().setScale({200, 50});
        nodes.emplace_back(bx);
        // nodes.emplace_back(btn);

        // DropdownPtr bx2 = bx->createSubMenuItem();
        // // DropdownPtr bx3 = bx->createSubMenuItem();

        // for (int32_t i = 0; i < 3; i++)
        // {
        //     ButtonWPtr item = bx->createMenuItem<Button>();
        //     item.lock()->setColor(Utils::randomRGB());
        // }

        // for (int32_t i = 0; i < 3; i++)
        // {
        //     ButtonPtr item = bx3->createMenuItem<Button>();
        //     item->setColor(Utils::randomRGB());
        // }
        // bx2->setColor(Utils::hexToVec4("#ffaa00ff"));
        // bx2->getLayout()
        //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        //     .setScale({1.0f, 20});

        // for (int32_t i = 0; i < 6; i++)
        // {
        //     ButtonPtr item = bx2->createMenuItem<Button>();
        //     item->setColor(Utils::randomRGB());
        // }

        // DropdownPtr bx3 = Utils::make<Dropdown>("Drop");
        // bx3->setColor(Utils::hexToVec4("#ffaa00ff"));
        // bx3->getLayout()
        //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
        //     .setScale({1.0f, 20})
        //     // .setMargin({1, 1, 0, 0})
        //     ;

        // for (int32_t i = 0; i < 6; i++)
        // {
        //     bx3->addMenuItem(Utils::randomRGB(), [i, mainLog, bx2, bx]()
        //     {
        //         if (i == 5)
        //         {
        //             bx->addSubMenuItem(bx2);
        //             mainLog.debugLn("added submenu");
        //         }
        //         else
        //         {
        //             mainLog.debugLn("called me button three");
        //         }
        //     });
        // }

        // bx->addSubMenuItem(bx2);
        // bx->addSubMenuItem(bx3);
        // bx2->addMenuItem(bx3);
    }

    // {
    //     DropdownPtr bx = Utils::make<Dropdown>("Drop");
    //     bx->setColor(Utils::hexToVec4("#ffaa00ff"));
    //     bx->getLayout()
    //         .setScaleType(Layout::ScaleType::ABS)
    //         .setScale({200, 50})
    //         .setMargin({10, 10, 60, 10});
    //     nodes.emplace_back(bx);

    //     for (int32_t i = 0; i < 3; i++)
    //     {
    //         bx->addMenuItem(Utils::randomRGB(), [mainLog]()
    //         {
    //             mainLog.debugLn("called me button");
    //         });
    //     }

    //     DropdownPtr bx2 = Utils::make<Dropdown>("Drop");
    //     bx2->setColor(Utils::hexToVec4("#ffaa00ff"));
    //     bx2->getLayout()
    //         .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
    //         .setScale({1.0f, 20})
    //         // .setMargin({1, 1, 0, 0})
    //         ;

    //     for (int32_t i = 0; i < 6; i++)
    //     {
    //         bx2->addMenuItem(Utils::randomRGB(), [mainLog]()
    //         {
    //             mainLog.debugLn("called me button two");
    //         });
    //     }

    //     DropdownPtr bx3 = Utils::make<Dropdown>("Drop");
    //     bx3->setColor(Utils::hexToVec4("#ffaa00ff"));
    //     bx3->getLayout()
    //         .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
    //         .setScale({1.0f, 20})
    //         // .setMargin({1, 1, 0, 0})
    //         ;

    //     for (int32_t i = 0; i < 6; i++)
    //     {
    //         bx3->addMenuItem(Utils::randomRGB(), [mainLog]()
    //         {
    //             mainLog.debugLn("called me button three");
    //         });
    //     }

    //     bx->addMenuItem(bx2);
    //     bx2->addMenuItem(bx3);
    // }
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