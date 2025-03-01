#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/RecycleList.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/nodeEvent/LMBItemRelease.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates the use of reyclelist node. This node tries to minimize the real number of displayed nodes
        and thus increasing performance when a large number of items need to be held in a list. Even if the user
        appends 1 million items, the list logic will be applied only to the visible list item nodes, which are
        usually MUCH less than 1 million (maybe 100-200 tops).
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER);

    RecycleListPtr rl = Utils::make<RecycleList>("MyRecycleList");
    rl->getLayout()
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({300, 0.5f});

    rootBox->append(rl);
    for (int32_t i = 0; i < 100; i++)
    {
        rl->addItem(Utils::randomRGB());
    }

    rl->getEvents().listen<nodeevent::LMBItemRelease>([mainLogger](const auto& evt)
    {
        mainLogger.debugLn("index clicked %u", evt.index);
    });
    mainLogger.debugLn("size of %d", sizeof(Button));

    /* Blocks from here on */
    app.run();

    return 0;
}