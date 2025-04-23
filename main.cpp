#include <string>

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
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({true, true})
        ;

    for (int32_t i = 0; i < 10; ++i)
    {
        ButtonPtr bigbox = Utils::make<Button>("mybox" + std::to_string(i));
        bigbox->getLayout()
            .setMargin({5})
            .setPadding({4})
            .setBorder({1})
            // .setNewScale({1_fit})
            .setNewScale({230_px , 60_px})
            ;

        bigbox->setColor(Utils::randomRGB())
            .setBorderColor(Utils::COLOR_BLACK)
            .setImagePath("assets/textures/awesomeface.png")
            .setText(bigbox->getName())
        ;

        bigbox->getEvents().listen<events::LMBRelease>(
            [mainLogger, &rootBox, ref = Utils::ref<Button>(bigbox)](const auto&)
            {
                rootBox->remove(ref.lock()->getId());
            });

        rootBox->append(bigbox);
    }



    // rootBox->append(bigbox);
    // rootBox->printTree();

    /* Blocks from here on */
    app.run();
    return 0;
}
