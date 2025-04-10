#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;
int main()
{
    /*
        Demonstrates use of buttons inside a box. Pretty simple.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLog{"mainLog"};

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAllowOverflow({true, true})
        .setAlignChild({Layout::Align::LEFT, Layout::Align::CENTER})
        .setSpacing(Layout::Spacing::EVEN_WITH_NO_START_GAP)
        .setType(Layout::Type::HORIZONTAL);

    AbstractNodePVec childButtons;
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonPtr btn = Utils::make<Button>("MyButton" + std::to_string(i));

        /* Elements are self-aligned based on the center line formed by the vertical scale of the button nodes
           and this also includes their vertical margins. Same can be said about vertically arranged buttons
           but obviously with self aligns flipped. */
        if (i == 0)      { btn->getLayout().setAlignSelf(Layout::Align::TOP).setMargin({10, 0}); }
        else if (i == 1) { btn->getLayout().setAlignSelf(Layout::Align::CENTER).setMargin({20, 0}); }
        else if (i == 2) { btn->getLayout().setAlignSelf(Layout::Align::BOTTOM).setMargin({20, 0}); }

        childButtons.emplace_back(btn);
    }
    rootBox->appendMany(childButtons);

    /* Let's find a button inside the box and attach a listener to it. Note that finding returns a generic node
       so we need to cast it to our expected node type. */
    ButtonPtr ourButton = rootBox->findOneBy<Button>([](const AbstractNodePtr& node)
    {
        return node->getName() == "MyButton1";
    });

    if (ourButton)
    {
        mainLog.debugLn("Found our button!");
        ourButton->getEvents().listen<events::LMBRelease>([mainLog](const auto&)
        {
            mainLog.debugLn("Clicked me");
        });
    }

    /* Let's now dynamically querry for the window frame, then, since we know all our children are buttons, disable
       clicks on all of them as long as the click is held on the parent box. On release, enable buttons back up. */
    rootBox->getEvents().listen<events::LMBClick>([](const auto&)
    {   
        auto pred = [](const auto& wf) -> bool
        {
            return wf->getRoot()->getName() == "MainWindow";
        };

        Application& appInner = Application::get();
        if (auto wf = appInner.getFrameBy(pred).lock())
        {
            for (auto& childButton : wf->getRoot()->getChildren())
            {
                Utils::as<Button>(childButton)->setEnabled(false);
            }
        }
    });

    rootBox->getEvents().listen<events::LMBRelease>([](const auto&)
    {
        Application& appInner = Application::get();
        if (auto wf = appInner.getFrameNamed("MainWindow").lock())
        {
            for (auto& childButton : wf->getRoot()->getChildren())
            {
                Utils::as<Button>(childButton)->setEnabled(true);
            }
        }
    });

    /* Blocks from here on */
    app.run();

    return 0;
}