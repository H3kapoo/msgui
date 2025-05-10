#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBItemRelease.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/RecycleList.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"
#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/events/LMBTreeItemRelease.hpp"
#include "msgui/events/LMBItemRelease.hpp"
#include <string>

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
        // .setAlignChild(Layout::Align::CENTER)
        ;

    RecycleListPtr rlLeft = Utils::make<RecycleList>("rlLeft");
    rlLeft->getLayout().setNewScale({1_fill, 1.0_rel});
    for (int32_t i = 0; i < 10; ++i)
    {
        rlLeft->addItem({Utils::randomRGB(), std::to_string(i)});
    }

    TreeViewPtr tw = Utils::make<TreeView>("tw");
    tw->getLayout().setNewScale({1_fill, 1.0_rel});

    TreeItemPtr root = Utils::make<TreeItem>();
    root->color = Utils::randomRGB();
    root->text = "Root";
    tw->addRootItem(root);

    for (int32_t i = 0; i < 10; ++i)
    {
        TreeItemPtr item = Utils::make<TreeItem>();
        item->color = Utils::randomRGB();
        item->text = std::to_string(i);
        root->addItem(item);
    }

    tw->getEvents().listen<events::LMBTreeItemRelease>(
        [mainLogger](const events::LMBTreeItemRelease& ev)
        {
            ev.item.lock()->text = "new text";

            // TreeItemPtr item = Utils::make<TreeItem>();
            // item->color = Utils::randomRGB();
            // item->text = "stefu";
            // ev.item.lock()->addItem(item);
        });

    TreeViewPtr twr = Utils::make<TreeView>("twr");
    twr->getLayout().setNewScale({1_fill, 1.0_rel});

    TreeItemPtr root2 = Utils::make<TreeItem>();
    root2->color = Utils::randomRGB();
    root2->text = "Root";
    twr->addRootItem(root2);

    for (int32_t i = 0; i < 1000; ++i)
    {
        TreeItemPtr item = Utils::make<TreeItem>();
        item->color = Utils::randomRGB();
        item->text = std::to_string(i);
        root2->addItem(item);
    }

    twr->getEvents().listen<events::LMBTreeItemRelease>(
        [mainLogger](const events::LMBTreeItemRelease& ev)
        {
            ev.item.lock()->text = "new text";

            // TreeItemPtr item = Utils::make<TreeItem>();
            // item->color = Utils::randomRGB();
            // item->text = "stefu";
            // ev.item.lock()->addItem(item);
        });


    BoxDividerPtr bd = Utils::make<BoxDivider>("bd");
    bd->getLayout().setNewScale({1_fill});
    bd->createSlots({0.25_rel, 0.5_rel, 0.25_rel});
    bd->getSlot(0).lock()->append(rlLeft);
    bd->getSlot(1).lock()->append(tw);
    bd->getSlot(2).lock()->append(twr);
    // rootBox->append(rlLeft);
    // rootBox->append(tw);
    // rootBox->append(twr);

    rootBox->append(bd);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
