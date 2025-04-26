#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/RMBRelease.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
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
        // .setPadding({5, 10, 5, 10})
        // .setBorder({4})
        ;

    BoxDividerPtr div = Utils::make<BoxDivider>("bd");
    div->getLayout().setNewScale({1_fill}).setType(Layout::Type::VERTICAL);
    const float cnt = 3;
    const float eq = 1.0f / cnt;
    div->createSlots(cnt, {eq, eq, eq});
    // for (int32_t i = 0; i < 2; ++i)
    // {
    //     BoxPtr bigbox = Utils::make<Box>("mybox" + std::to_string(i));
    //     bigbox->getLayout()
    //         // .setAlignSelf(Layout::CENTER_BOTTOM)
    //         .setNewScale({1_fill})
    //         // .setNewScale({100_px})
    //         // .setNewScale({1_fill})
    //     ;

    //     bigbox->setColor(Utils::randomRGB());

    //     rootBox->append(bigbox);
    // }

    rootBox->append(div);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
