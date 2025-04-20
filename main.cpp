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
        .setAllowWrap(true)
        .setAlignChild(Layout::Align::TOP_LEFT)
        // .setType(Layout::Type::VERTICAL)
        // .setAlignChild(Layout::Align::CENTER)
        ;

    for (int32_t i = 0; i < 5; ++i)
    {
        BoxPtr bigbox = Utils::make<Box>("mybox" + std::to_string(i));
        bigbox->setColor(Utils::randomRGB());
        bigbox->getLayout()
            .setAlignSelf(Layout::Align::CENTER)
            // .setNewScale({1_fill, 1_fill})
            .setNewScale({80_px * Utils::randomInt(1, 3), 70_px * Utils::randomInt(1, 3)})
            ;

            if (i == 3)
            {
                bigbox->getLayout()
                    .setType(Layout::Type::VERTICAL)
                    .setAllowWrap(true)
                    // .setNewScale({0.3_rel, 1_fit});
                    .setNewScale({0.3_rel, 0.5_rel});
                    // .setNewScale({1_fit, 0.5_rel});
                for (int32_t j = 1; j < 6; ++j)
                {
                    BoxPtr b = Utils::make<Box>("mybox 2" + std::to_string(j));
                    b->setColor(Utils::randomRGB());
                    b->getLayout()
                        // .setNewScale({50_px, 50_px});
                        .setNewScale({50_px * Utils::randomInt(1, 5), 50_px * Utils::randomInt(1, 3)});

                    bigbox->append(b);

                    mainLogger.debugLn("size is %f %f", b->getLayout().newScale.x.value,
                    b->getLayout().newScale.y.value);
                }
            }
        rootBox->append(bigbox);
    }


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

    // rootBox->append(bigbox);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
