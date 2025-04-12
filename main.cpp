#include <string>

#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/MouseEnter.hpp"
#include "msgui/events/MouseExit.hpp"
#include "msgui/events/Scroll.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#252525ff"));
    rootBox->getLayout()
        // .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({true, true})
        .setAlignChild(Layout::Align::CENTER);

    rootBox->getVBar().lock()->setSensitivity(5);

    BoxPtr a, b;
    a = Utils::make<Box>("leftbox");
    a->setColor(Utils::randomRGB());
    a->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({true, true})
        .setScale({400, 200})
        .setMargin({0, 0, 0, 5});

    b = Utils::make<Box>("rightbox");
    b->setColor(Utils::randomRGB());
    b->getLayout()
        .setAllowOverflow({true, true})
        .setScale({300, 200});

    for (int32_t i = 0; i < 20; i++)
    {
        ButtonPtr btn = Utils::make<Button>("mbutton");
        btn->setColor(Utils::hexToVec4("#cf1616ff"))
            .setText("My text is very huge and it will not fit in but im trying it anyway")
        ;
        btn->getLayout().setScale({300, 40});
        
        if (i < 10)
        {
            a->append(btn);
        }
        else
        {
            b->append(btn);
        }
        // rootBox->appendMany({b});
    }

    rootBox->appendMany({a, b});


    // rootBox->getEvents().listen<events::MouseExit>(
    //     [&mainLogger](const auto&)
    //     {
    //         mainLogger.debugLn("Something exit");
    //     });
    // rootBox->getEvents().listen<events::MouseEnter>(
    //     [&mainLogger](const auto&)
    //     {
    //         mainLogger.debugLn("Something entered");
    //     });
    // rootBox->append(lbl);

    /* Blocks from here on */
    // Application::get().setPollMode(Application::PollMode::CONTINUOUS);
    app.run();
    return 0;
}
