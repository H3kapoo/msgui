#include "msgui/Texture.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/events/LMBItemRelease.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBTreeItemRelease.hpp"

#include <chrono>
#include <string>
#include <thread>

using namespace msgui;
using namespace msgui::layoutengine::utils;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#252525ff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER);

    // TextLabelPtr lbl = Utils::make<TextLabel>("Text");
    // lbl->getLayout().setScale({256, 256});
    // lbl->setText("Learn OpenGL.com")
    //     .setFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/LiberationSerif-Regular.ttf")
    //     .setFontSize(30);

    // lbl->getEvents().listen<events::LMBRelease>(
    //     [ref = Utils::ref<TextLabel>(lbl), mainLogger](const auto&)
    //     {
    //         // mainLogger.debugLn("pe aici");
    //         ref.lock()->setText("Lorem ipsum dolor sit amet, consectetur afeefefef");
    //         ref.lock()->setFontSize(16);
    //     });
    // rootBox->append(lbl);

    TreeViewPtr tv = Utils::make<TreeView>("myTreeView");
    tv->getLayout().setScale({300, 300});
    tv->getLayout()
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({0.3f, 0.5f});
    tv->setItemBorder({1});

    rootBox->append(tv);

    TreeItemPtr root = Utils::make<TreeItem>();
    root->color = {0, 0, 0, 1};
    root->stringInfo = "Root";
    
    // TreeItemPtr rootCH1 = Utils::make<TreeItem>();
    // rootCH1->color = {1, 0, 0, 1};
    // rootCH1->stringInfo = "Root_CH_1";
    // root->addItem(rootCH1);
    
    for (int i = 0; i < 10; i++)
    {
        TreeItemPtr rootCH2 = Utils::make<TreeItem>();
        rootCH2->color = {1, 0, 1, 1};
        rootCH2->stringInfo = "Root_CH_2_id-bla" + std::to_string(i);
        // rootCH2->isOpen = true;
        root->addItem(rootCH2);

        if (i % 3)
        {
            TreeItemPtr blaItem = Utils::make<TreeItem>();
            blaItem->color = Utils::randomRGB();
            blaItem->stringInfo = "Ceva-" + std::to_string(i);
            rootCH2->addItem(blaItem);
            
            // if (i % 4 == 1)
            // {
            //     TreeItemPtr uhItem = Utils::make<TreeItem>();
            //     uhItem->color = Utils::randomRGB();
            //     uhItem->stringInfo = "wow-" + std::to_string(i);
            //     blaItem->addItem(uhItem);
            // }
        }
    }

    // TreeItemPtr CH1_CH1 = Utils::make<TreeItem>();
    // CH1_CH1->color = {1, 0.5f, 0.5f, 1};
    // CH1_CH1->stringInfo = "wow";
    // rootCH1->addItem(CH1_CH1);
    
    TreeItemPtr root2 = Utils::make<TreeItem>();
    root2->color = {0, 0, 1, 1};
    root2->stringInfo = "Root2";
    
    // TreeItemPtr root2CH1 = Utils::make<TreeItem>();
    // root2CH1->color = {1, 0, 1, 1};
    // root2CH1->stringInfo = "Ceva";
    // root2->addItem(root2CH1);

    tv->addRootItem(root);
    // tv->addRootItem(root2);

    tv->getEvents().listen<events::LMBTreeItemRelease>(
        [ref = Utils::ref<TreeView>(tv), mainLogger](const auto& evt)
        {
            mainLogger.debugLn("clicked on %s", evt.item.lock()->stringInfo.c_str());
            // TreeItemPtr it = evt.item.lock();
            // it->stringInfo = "altcv";
        });

    /* Blocks from here on */
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    // app.setVSync(false);
    app.run();
    return 0;
}
