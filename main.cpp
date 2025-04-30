#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/Button.hpp"
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
    rootBox->setColor(Utils::hexToVec4("#6e6e6eff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER)
        // .setPadding({5, 10, 5, 10})
        // .setBorder({4})
        ;
    BoxPtr container = Utils::make<Box>("the box");
    container->getLayout()
        .setNewScale({0.90_rel})
        .setType(Layout::Type::HORIZONTAL);

    BoxDividerPtr div = Utils::make<BoxDivider>("bd");
    div->getLayout()
        .setNewScale({1_fill})
        .setType(Layout::Type::HORIZONTAL)
        // .setBorder({5})
        // .setAllowOverflow({true, true})
        ;
    // div->createSlots(cnt, {eq, eq, eq});
    div->createSlots({0.25_rel, 0.5_rel, 0.25_rel});

    BoxPtr slot0 = div->getSlot(0).lock();
    BoxPtr slot1 = div->getSlot(1).lock();
    BoxPtr slot2 = div->getSlot(2).lock();

    slot0->getLayout()
        .setMinScale({200, 1})
        .setMaxScale({300, 10000})
    ;
    // slot1->getLayout()
    //     .setMinScale({200, 1})
    //     .setMaxScale({300, 10000})
    ;
    slot2->getLayout()
        .setMinScale({200, 1})
        .setMaxScale({300, 10000})
    ;
    // slot1->getLayout().setMinScale({200, 1});
    // slot2->getLayout().setMinScale({200, 1});
    // slot1->getLayout().setMargin({5});

    // BoxDividerSepPtr sep1 = div->getSepatator(0).lock();
    // sep1->setColor(Utils::COLOR_CYAN);
    // sep1->getLayout().setMargin({5});
    // for (int32_t i = 0; i < 2; ++i)
    // {
    //     BoxPtr bigbox = Utils::make<Box>("mybox" + std::to_string(i));
    //     bigbox->getLayout()
    //         // .setAlignSelf(Layout::CENTER_BOTTOM)
    //         .setNewScale({1_fill})
    //         // .setNewScale({100_px})
    //         // .setNewScale({1_fill})
    //     ;

    //     bigbox->setColor(Utils::randomRGB());

    //     rootBox->append(bigbox);
    // }

    container->append(div);
    rootBox->append(container);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
