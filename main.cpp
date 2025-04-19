#include <string>

#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
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
        .setAlignChild(Layout::Align::CENTER);

    BoxPtr bigbox = Utils::make<Box>("mybox");
    // bigbox->setColor(Utils::COLOR_RED);
    bigbox->getLayout()
        .setType(Layout::Type::VERTICAL)
        // .setNewScale({1_fill, 1_fill})
        .setNewScale({400_px, 0.45_rel})
        ;

    // {
    //     BoxPtr b = Utils::make<Box>("mybox 2");
    //     b->setColor(Utils::randomRGB());
    //     b->getLayout()
    //         // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
    //         .setNewScale({1_fill, 200_px});
    //     bigbox->append(b);
    // }

    // {
    //     BoxPtr b = Utils::make<Box>("mybox 3");
    //     b->setColor(Utils::randomRGB());
    //     b->getLayout()
    //         .setNewScale({0.5_rel});
    //     bigbox->append(b);
    // }

    // {
    //     BoxPtr b = Utils::make<Box>("mybox 4");
    //     b->setColor(Utils::randomRGB());
    //     b->getLayout()
    //         // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
    //         // .setScale({0.1f, 1.0f})
    //         .setScale({1.0f, 0.3f})
    //         .setScaleType(Layout::ScaleType::REL);
    //         // .setScaleType({Layout::ScaleType::FILL, Layout::ScaleType::FILL});
    //         bigbox->append(b);
    // }

    // {
    //     BoxPtr b = Utils::make<Box>("mybox");
    //     b->setColor(Utils::randomRGB());
    //     b->getLayout()
    //         // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
    //         .setScale({250, 250})
    //         // .setScaleType(Layout::ScaleType::PX);
    //         .setScaleType({Layout::ScaleType::FILL, Layout::ScaleType::FILL});
    //         bigbox->append(b);
    // }

    rootBox->append(bigbox);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
