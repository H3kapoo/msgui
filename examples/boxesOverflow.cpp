#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/node/utils/ScrollBar.hpp"
#include "core/nodeEvent/LMBRelease.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates overflow on both axis. Margins and paddings do affect overflow occurance.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLog("mainLogger");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAllowOverflow({true, true})
        .setPadding({0, 0, 40, 40})
        .setType(Layout::Type::HORIZONTAL);

    /* Change thickness of the scrollbar. In this case we are 100% it exists but we don't
       wanna take ownership of it. */
    ScrollBarWPtr hBar = rootBox->getHBar();
    hBar.lock()->setScrollbarSize(40);

    AbstractNodePVec childBoxes;
    for (int32_t i = 0; i < 4; i++)
    {
        BoxPtr box = Utils::make<Box>("MyBoxName" + std::to_string(i));
        box->setColor(Utils::randomRGB());
        box->getLayout()
            .setType(Layout::Type::HORIZONTAL)
            .setScaleType(Layout::ScaleType::ABS)
            .setScale({400, 200})
            .setMargin({5});
        childBoxes.emplace_back(box);
    }
    rootBox->appendMany(childBoxes);

    /* Add a listener to the root box to toggle horizontal overflow and change scrollbar size
       when we release the click on it. */
    rootBox->getEvents().listen<LMBRelease>(
        [mainLog, ref = Utils::ref<Box>(rootBox)](const auto&)
        {
            if (auto l = ref.lock())
            {
                auto& layout = l->getLayout();
                layout.allowOverflow.x ?
                    layout.setAllowOverflow({false, true}) :
                    layout.setAllowOverflow({true, true});

                if (auto hBarLock = l->getHBar().lock())
                {
                    hBarLock->setScrollbarSize(20);
                }
            }
        });

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}