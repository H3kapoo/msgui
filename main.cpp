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
    BoxPtr aa = Utils::make<Box>("aa");
    aa->setColor(Utils::COLOR_CYAN);
    aa->getLayout().setScale({300, 300});

    DropdownPtr dd = aa->createContextMenu().lock();
    // dd->getLayout()
    //     .setScale({200, 50})
    //     ;
    rootBox->appendMany({aa});

    dd->getContainer().lock()->setColor(Utils::lighten(Utils::COLOR_RED, 0.2f));
    for (int32_t i = 0; i< 3; i++)
    {
        dd->createMenuItem<Button>().lock()->setText("what " + std::to_string(i))
            .getTextLabel().lock()->setTextColor(Utils::randomRGB());
    }

    rootBox->printTree();
    /* Blocks from here on */
    // Application::get().setPollMode(Application::PollMode::CONTINUOUS);
    app.run();
    return 0;
}
