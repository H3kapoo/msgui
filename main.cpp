#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/Button.hpp"
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
    rootBox->setColor(Utils::hexToVec4("#ffffffff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER)
        // .setPadding({5, 10, 5, 10})
        // .setBorder({4})
        .setPadding({1})
        ;
    
    int x = 3;
    for (int i = 0; i < x; ++i)
    {
        BoxPtr box = Utils::make<Box>("box");
        box->setColor(Utils::randomRGB());
        box->getLayout().setNewScale({0.25_rel, 1.0_rel});
        box->getLayout().newScale.x.value = 1.0f / x;
        
        rootBox->append(box);
    }
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
