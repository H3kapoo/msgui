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
        .setAlignChild(Layout::Align::CENTER)
        ;

    RecycleListPtr rl = Utils::make<RecycleList>("rl");
    rl->getLayout().setNewScale({0.5_rel, 0.5_rel}).setPadding({1});

    for (int32_t i = 0; i < 100; i++)
    {
        rl->addItem({Utils::randomRGB(), std::to_string(i)});
    }

    rl->getEvents().listen<events::LMBItemRelease>([mainLogger](const auto& ev)
    {
        mainLogger.debugLn("item %s", ev.item->text.c_str());
        ev.item->text = "ceva";
    });

    rootBox->append(rl);
    rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
