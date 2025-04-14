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
#include "msgui/node/Dropdown.hpp"
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
    rootBox->setColor(Utils::hexToVec4("#161616ff"));
    rootBox->getLayout()
        // .setType(Layout::Type::VERTICAL)
        // .setAllowOverflow({true, true})
        .setAlignChild(Layout::Align::CENTER);

    // rootBox->getVBar().lock()->setSensitivity(5);

    DropdownPtr dd = Utils::make<Dropdown>("down");
    dd->setImagePath("/home/hekapoo/Documents/probe/newgui/assets/textures/awesomeface.png");
    dd->setText("my text");
    dd->getLayout()
        .setScale({200, 60})
        ;

    dd->getImage().lock()->getLayout().setAlignSelf(Layout::Align::CENTER);
    rootBox->appendMany({dd});

    dd->getContainer().lock()->setColor(Utils::lighten(Utils::COLOR_RED, 0.2f));

    for (int32_t i = 0; i < 2; i++)
    {
        ButtonWPtr bb = dd->createMenuItem<Button>();
        // bb.lock()->getLayout().setMargin({2});
        // bb.lock()->getLayout().setScale({150, 60});
        bb.lock()->setColor(Utils::randomRGB()).setText("Ceva text");
    }

    // DropdownWPtr sub = dd->createSubMenuItem();
    // sub.lock()->setColor(Utils::randomRGB());
    // sub.lock()->getLayout().setMargin({2});
    
    // for (int32_t i = 0; i < 3; i++)
    // {
    //     ButtonWPtr bb = sub.lock()->createMenuItem<Button>();
    //     bb.lock()->getLayout().setMargin({2});
    //     bb.lock()->setColor(Utils::randomRGB());
    // }

    /* Blocks from here on */
    // Application::get().setPollMode(Application::PollMode::CONTINUOUS);
    app.run();
    return 0;
}
