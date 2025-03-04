#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

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
    rootBox->getLayout()
        .setSpacing(Layout::Spacing::EVEN_WITH_NO_START_GAP)
        .setAlignChild(Layout::Align::CENTER);

    for (int i = 0; i < 2; i++)
    {
        BoxPtr box = Utils::make<Box>("newBox");
        box->getLayout().setScale({200, 200});
        box->setColor(Utils::randomRGB());
        rootBox->append(box);

        DropdownWPtr ddCtx = box->createContextMenu();
        ddCtx.lock()->setExpandDirection(Dropdown::Expand::LEFT);
        ddCtx.lock()->getContainer().lock()->setColor(Utils::randomRGB());
        for (int j = 0; j < 2; j++)
        {
            auto btn = ddCtx.lock()->createMenuItem<Button>();
            btn.lock()->getLayout().setScale({100, 34});
            btn.lock()->setColor(Utils::randomRGB());
            btn.lock()->getLayout().setBorder({6, 6});

            btn.lock()->getEvents().listen<nodeevent::LMBRelease>(
                [ref = Utils::ref<Box>(box)](const auto&)
            {
                ref.lock()->setColor(Utils::randomRGB());
            });
        }
    }

    /* Blocks from here on */
    app.run();

    return 0;
}