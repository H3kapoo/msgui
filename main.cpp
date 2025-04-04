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
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/node/utils/TreeItem.hpp"
#include "msgui/nodeEvent/LMBItemRelease.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/LMBTreeItemRelease.hpp"

#include <chrono>
#include <thread>

using namespace msgui;

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

    TextLabelPtr lbl = Utils::make<TextLabel>("Text");
    lbl->getLayout().setScale({256, 256});
    lbl->setText("Learn OpenGL.com")
        .setFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/LiberationSerif-Regular.ttf")
        .setFontSize(30);

    lbl->getEvents().listen<nodeevent::LMBRelease>(
        [ref = Utils::ref<TextLabel>(lbl), mainLogger](const auto&)
        {
            // mainLogger.debugLn("pe aici");
            ref.lock()->setText("Lorem ipsum dolor sit amet, consectetur a");
            // ref.lock()->setFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/cmr10.ttf");
            ref.lock()->setFontSize(16);
            // ref.lock()->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc, quis gravida magna mi a libero. Fusce vulputate eleifend sapien. Vestibulum purus quam, scelerisque ut, mollis sed, nonummy id, metus. Nullam accumsan lorem in dui. Cras ultricies mi eu turpis hendrerit fringilla. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; In ac dui quis mi consectetuer lacinia. Nam pretium turpis et arcu. Duis arcu tortor, suscipit eget, imperdiet nec, imperdiet iaculis, ipsum. Sed aliquam ultrices mauris. Integer ante arcu, accumsan a, consectetuer eget, posuere ut, mauris. Praesent adipiscing. Phasellus ullamcorper ipsum rutrum nunc. Nunc nonummy metus. Vestibulum volutpat pretium libero. Cras id dui. Aenean ut eros et nisl sagittis vestibulum. Nullam nulla eros, ultricies sit amet, nonummy id, imperdiet feugiat, pede. Sed lectus. Donec mollis hendrerit risus. Phasellus nec sem in justo pellentesque facilisis. Etiam imperdiet imperdiet orci. Nunc nec neque. Phasellus leo dolor, tempus ");
        });
    rootBox->append(lbl);

    std::thread t([lbl]()
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        lbl->setFontSize(80);
    });

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
    // app.setPollMode(Application::PollMode::CONTINUOUS);
    // app.setVSync(false);
    app.run();
    t.join();
    return 0;

}
