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

    /* Create two random boxes */
    for (int i = 0; i < 2; i++)
    {
        BoxPtr box = Utils::make<Box>("newBox");
        box->getLayout().setScale({200, 200});
        box->setColor(Utils::randomRGB());
        rootBox->append(box);
        
        /* Spawn a context menu and set it up as you like */
        DropdownWPtr ddCtx = box->createContextMenu();
        ddCtx.lock()->setExpandDirection(Dropdown::Expand::RIGHT);
        ddCtx.lock()->getContainer().lock()->setColor(Utils::randomRGB());
        ddCtx.lock()->getContainer().lock()->getLayout().setBorder({1, 2});

        /* It acts as a normal dropdown so you can add menu items as you go */
        for (int j = 0; j < 6; j++)
        {
            auto btn = ddCtx.lock()->createMenuItem<Button>();
            btn.lock()->getLayout().setScale({100, 34});
            btn.lock()->setColor(Utils::randomRGB());
            btn.lock()->getLayout().setBorder({1, 0});

            btn.lock()->getEvents().listen<nodeevent::LMBRelease>(
                [ref = Utils::ref<Box>(box)](const auto&)
            {
                ref.lock()->setColor(Utils::randomRGB());
            });
        }
    }

    /* We know the root box as two boxes inside. Remove the context menu of the first box
       just because :) */
    BoxPtr boxOne = Utils::as<Box>(rootBox->getChildren()[0]);
    boxOne->removeContextMenu();

    /* Now put it back, but different */
    DropdownWPtr ddCtx = boxOne->createContextMenu();
    ddCtx.lock()->getContainer().lock()->getLayout().setBorder({1, 2});
    for (int j = 0; j < 4; j++)
    {
        auto btn = ddCtx.lock()->createMenuItem<Button>();
        btn.lock()->getLayout().setScale({100, 34});
        btn.lock()->setColor(Utils::randomRGB());
        btn.lock()->getLayout().setBorder({1, 0});

        btn.lock()->getEvents().listen<nodeevent::LMBRelease>(
            [ref = Utils::ref<Box>(boxOne)](const auto&)
        {
            ref.lock()->setColor(Utils::randomRGB());
        });
    }

    /* Blocks from here on */
    app.run();

    return 0;
}
