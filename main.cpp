#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

using namespace msgui;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER);

    TreeViewPtr tv = Utils::make<TreeView>("myTreeView");
    tv->getLayout().setScale({300, 300});
    // tv->getLayout()
    //     .setScaleType(Layout::ScaleType::REL)
    //     .setScale({1.0f, 1.0f});
    tv->setItemBorder({1});

    rootBox->append(tv);

    TreeItemPtr root = Utils::make<TreeItem>();
    root->color = {0, 0, 0, 1};

    TreeItemPtr rootCH1 = Utils::make<TreeItem>();
    rootCH1->color = {1, 0, 0, 1};
    root->addItem(rootCH1);

    for (int i = 0; i < 20; i++)
    {
        TreeItemPtr rootCH2 = Utils::make<TreeItem>();
        rootCH2->color = {1, 1, 0, 1};
        root->addItem(rootCH2);

        if (i % 3)
        {
            TreeItemPtr blaItem = Utils::make<TreeItem>();
            blaItem->color = Utils::randomRGB();
            rootCH2->addItem(blaItem);
        }
    }

    TreeItemPtr CH1_CH1 = Utils::make<TreeItem>();
    CH1_CH1->color = {1, 1, 1, 1};
    rootCH1->addItem(CH1_CH1);

    TreeItemPtr root2 = Utils::make<TreeItem>();
    root2->color = {0, 0, 1, 1};

    TreeItemPtr root2CH1 = Utils::make<TreeItem>();
    root2CH1->color = {1, 0, 1, 1};
    root2->addItem(root2CH1);

    tv->addItem(root);
    tv->addItem(root2);
    // tv->printTreeView();

    /* Blocks from here on */
    app.run();

    return 0;

    /*
        0
        - 1
          - 2
          - 3
        - 4
        5
        6

            x
       0     5   6  
    1    4
   2 3
    */
}
