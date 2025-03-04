#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include <string>

using namespace msgui;

int main()
{
    /*
        Demonstrates the use of context menus (right clicking on stuff). These are just plain old
        dropdowns specially tailored for showing menus on top of nodes. For now, only Boxes support
        context menus.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    // rootBox->getLayout()
        // .setSpacing(Layout::Spacing::EVEN_WITH_NO_START_GAP)
        // .setAlignChild(Layout::Align::BOTTOM);

    for (int32_t i = 0; i < 4; i++)
    {
        DropdownPtr dd = Utils::make<Dropdown>("myDd" + std::to_string(i));
        dd->setColor(Utils::hexToVec4("#414141ff"))
            // .setExpandDirection(Dropdown::Expand::TOP)
            ;
        dd->getLayout()
            .setScale({74, 30})
            // .setScale({200, 60})
            .setBorder({0, 1, 0, 1})
            // .setBorderRadius({1})
            ;

        if (i == 3)
        {
            dd->getLayout()
                .setBorderRadius({0, 0, 4, 0});
        }

        for (int32_t j = 0; j < 3; j++)
        {
            ButtonWPtr btn = dd->createMenuItem<Button>();
            btn.lock()->setColor(Utils::hexToVec4("#303030ff"));
            btn.lock()->getLayout()
                .setScale({150, 30})
                .setBorder({1})
                ;
        }

        rootBox->append(dd);
    }
    /* Blocks from here on */
    app.run();

    return 0;
}
