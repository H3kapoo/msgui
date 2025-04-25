#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
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
        .setType(Layout::Type::GRID)
        // .setAllowOverflow({true, true})
        .setGridDistrib({{1_fr, 1_fr, 2_fr}, {1_fr, 1_fr}})
        ;

    for (int32_t i = 0; i < 3; ++i)
    {
        for (int32_t j = 0; j < 2; ++j)
        {
            BoxPtr bigbox = Utils::make<Box>("mybox" + std::to_string(i));
            bigbox->getLayout()
                .setGridPosRC({i, j})
                // .setAlignSelf(Layout::CENTER_BOTTOM)
                // .setNewScale({60_px})
                .setNewScale({1_fill})
            ;
    
            bigbox->setColor(Utils::randomRGB());
    
            rootBox->append(bigbox);
        }
    }

    // rootBox->append(bigbox);
    // rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
