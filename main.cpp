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
    bigbox->getLayout()
        .setType(Layout::Type::VERTICAL)
        // .setScale({400, 300})
        // .setScale({0.55f, 0.3f})
        .setScale({0.35f, 0.55f})
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        // .setScaleType({Layout::ScaleType::FIT, Layout::ScaleType::FIT})
        ;

    {
        BoxPtr b = Utils::make<Box>("mybox 2");
        b->setColor(Utils::randomRGB());
        b->getLayout()
            // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
            .setScale({250, 250})
            // .setScaleType(Layout::ScaleType::PX);
            .setScaleType({Layout::ScaleType::FILL, Layout::ScaleType::FILL});
            bigbox->append(b);
    }

    {
        BoxPtr b = Utils::make<Box>("mybox 3");
        b->setColor(Utils::randomRGB());
        b->getLayout()
            // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
            .setScale({250, 250})
            .setScaleType(Layout::ScaleType::PX);
            // .setScaleType({Layout::ScaleType::FILL, Layout::ScaleType::FILL});
            bigbox->append(b);
    }

    {
        BoxPtr b = Utils::make<Box>("mybox 4");
        b->setColor(Utils::randomRGB());
        b->getLayout()
            // .setScale({Utils::random01()*200.0f + 50, Utils::random01()*200.0f + 50})
            // .setScale({0.1f, 1.0f})
            .setScale({1.0f, 0.1f})
            .setScaleType(Layout::ScaleType::REL);
            // .setScaleType({Layout::ScaleType::FILL, Layout::ScaleType::FILL});
            bigbox->append(b);
    }

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
