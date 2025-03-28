#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/nodeEvent/LMBItemRelease.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/LMBTreeItemRelease.hpp"

using namespace msgui;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    // rootBox->getLayout()
    //     .setAlignChild(Layout::Align::CENTER);

    ImagePtr img = Utils::make<Image>("myimg");
    img->getLayout()
        .setScale({46*2, 46*2});
        // .setScale({128, 128});
        // .setScale({64, 64});
    rootBox->append(img);
    // TreeViewPtr tv = Utils::make<TreeView>("myTreeView");
    // tv->getLayout().setScale({300, 300});
    // tv->getLayout()
    //     .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL}) /
    //     .setScale({0.3f, 0.5f});
    // tv->setItemBorder({1});

    // rootBox->append(tv);

    // TreeItemPtr root = Utils::make<TreeItem>();*M
    // root->color = {0, 0, 0, 1};
    
    // TreeItemPtr rootCH1 = Utils::make<TreeItem>();
    // rootCH1->color = {1, 0, 0, 1};
    // root->addItem(rootCH1);
    
    // for (int i = 0; i < 20; i++)
    // {
    //     TreeItemPtr rootCH2 = Utils::make<TreeItem>();
    //     rootCH2->color = {1, 1, 0, 1};
    //     root->addItem(rootCH2);

    //     if (i % 3)
    //     {
    //         TreeItemPtr blaItem = Utils::make<TreeItem>();
    //         blaItem->color = Utils::randomRGB();
    //         rootCH2->addItem(blaItem);

    //         if (i % 4 == 1)
    //         {
    //             TreeItemPtr uhItem = Utils::make<TreeItem>();
    //             uhItem->color = Utils::randomRGB();
    //             blaItem->addItem(uhItem);
    //         }
    //     }
    // }

    // TreeItemPtr CH1_CH1 = Utils::make<TreeItem>();
    // CH1_CH1->color = {1, 1, 1, 1};
    // rootCH1->addItem(CH1_CH1);
    
    // TreeItemPtr root2 = Utils::make<TreeItem>();
    // root2->color = {0, 0, 1, 1};
    
    // TreeItemPtr root2CH1 = Utils::make<TreeItem>();
    // root2CH1->color = {1, 0, 1, 1};
    // root2->addItem(root2CH1);

    // tv->addRootItem(root);
    // tv->addRootItem(root2);

    // tv->getEvents().listen<nodeevent::LMBTreeItemRelease>([mainLogger](const auto& evt)
    // {
    //     mainLogger.debugLn("clicked on %d", evt.item.lock()->depth);
    // });

    /* Blocks from here on */
    app.setPollMode(Application::PollMode::CONTINUOUS);
    app.run();

    return 0;

}
