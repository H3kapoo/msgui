#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"
#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/events/LMBTreeItemRelease.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#282828ff"));
    rootBox->getLayout()
        .setPadding({1})
        ;

    DropdownPtr dd = Utils::make<Dropdown>("mydd");
    dd->setExpandDirection(Dropdown::Expand::BOTTOM);
    dd->setColor(Utils::hexToVec4("#282828ff"));
    dd->setText("Menu");
    dd->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
    dd->setItemSize({150_px, 34_px});
    dd->getLayout().setBorder({1});
    dd->getContainer().lock()->getLayout().setPadding({1, 0, 1, 1});

    {
        ButtonPtr b = dd->createMenuItem<Button>().lock();
        b->setColor(Utils::hexToVec4("#282828ff"));
        b->setText("New");
        b->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
        b->getLayout().setMargin({0, 1, 0, 0});
        b->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
    }

    {
        ButtonPtr b = dd->createMenuItem<Button>().lock();
        b->setColor(Utils::hexToVec4("#282828ff"));
        b->getLayout().setMargin({0, 1, 0, 0});
        b->setText("Open");
        b->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
        b->getEvents().listen<events::LMBRelease>([mainLogger](const auto&)
        {
            mainLogger.infoLn("i've been released");
        });

        DropdownPtr dd2 = dd->createSubMenuItem().lock();
        dd2->setItemSize({150_px, 34_px});
        dd2->setText("More..");
        dd2->setColor(Utils::hexToVec4("#282828ff"));
        dd2->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
        dd2->getLayout().setMargin({0, 1, 0, 0});
        dd2->setExpandDirection(Dropdown::Expand::RIGHT);
        dd2->getContainer().lock()->getLayout().setPadding({1});
        {
            ButtonPtr c = dd2->createMenuItem<Button>().lock();
            c->setColor(Utils::hexToVec4("#282828ff"));
            c->setText("Option");
            c->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
        }
    }

    {
        ButtonPtr b = dd->createMenuItem<Button>().lock();
        b->setText("Exit");
        b->getTextLabel().lock()->setTextColor(Utils::hexToVec4("#bebdb7ff"));
        // b->setEnabled(false);
        b->setColor(Utils::hexToVec4("#282828ff"));
        b->getLayout().setMargin({0, 1, 0, 0});
    }

    rootBox->append(dd);

    // BoxDividerPtr div = Utils::make<BoxDivider>("bd");
    // div->getLayout()
    //     .setNewScale({1_fill})
    //     // .setNewScale({0.9_rel})
    //     // .setNewScale({1000_px, 500_px})
    //     .setType(Layout::Type::HORIZONTAL)
    //     .setPadding({1})
    //     ;
    // div->createSlots({0.5_rel, 0.25_rel, 0.25_rel});
    // // div->createSlots({0.5_rel, 0.5_rel});

    // BoxPtr slot0 = div->getSlot(0).lock();
    // BoxPtr slot1 = div->getSlot(1).lock();
    // // BoxPtr slot2 = div->getSlot(2).lock();

    // slot0->getLayout()
    //     .setMinScale({100, 1})
    //     // .setMaxScale({300, 10000})
    // ;

    // // slot2->getLayout()
    // //     .setMinScale({200, 1})
    //     // .setMaxScale({300, 10000})
    // // ;

    // BoxDividerPtr divLeft = Utils::make<BoxDivider>("leftbox");
    // divLeft->getLayout()
    //     .setNewScale({1_fill})
    //     .setType(Layout::Type::VERTICAL)
    //     ;

    // divLeft->createSlots({0.5_rel, 0.5_rel});

    // slot0->append(divLeft);

    // rootBox->append(div);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
