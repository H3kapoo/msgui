#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/nodeEvent/LMBRelease.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates use of BoxDivider node and how user can create mouse draggable panels.
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

    /* Add a listener to the root box to toggle horizontal overflow when we release the click on it. */
    rootBox->getEvents().listen<LMBRelease>(
        [mainLog, ref = Utils::ref<Box>(rootBox)](const auto&)
        {
            if (auto l = ref.lock())
            {
                auto& layout = l->getLayout();
                layout.allowOverflow.x ?
                    layout.setAllowOverflow({false, true}) :
                    layout.setAllowOverflow({true, true});
            }
        });

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}