#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/BoxDivider.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/LMBRelease.hpp"

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

    BoxDividerPtr boxDivider = Utils::make<BoxDivider>("MyBoxDivider");
    boxDivider->getLayout()
        .setScaleType(utils::Layout::ScaleType::REL)
        .setScale({1.0f, 1.0f});

    rootBox->append(boxDivider);

    /* Create some slots and change their color. */
    const int32_t slotNo = 3;
    const float oneThird = 1.0f / slotNo;
    boxDivider->createSlots(3, {oneThird, oneThird, oneThird});

    boxDivider->getSlot(0).lock()->setColor(Utils::randomRGB());
    boxDivider->getSlot(1).lock()->setColor(Utils::randomRGB());
    boxDivider->getSlot(2).lock()->setColor(Utils::randomRGB());

    /* Change colors of the separators. */
    boxDivider->getSepatator(0).lock()->setColor(Utils::hexToVec4("#ddddddff"));
    boxDivider->getSepatator(1).lock()->setColor(Utils::hexToVec4("#ddddddff"));

    /* Set minimum widths for the slots, in pixels. */
    boxDivider->getSlot(0).lock()->getLayout().setMinScale({200, 0});
    boxDivider->getSlot(1).lock()->getLayout().setMinScale({100, 0});
    boxDivider->getSlot(2).lock()->getLayout().setMinScale({100, 0});

    /* Some dragging action on the first slot maybe? */
    boxDivider->getSlot(0).lock()->getEvents().listen<events::LMBDrag>(
        [mainLog](const auto& evt)
        {
            mainLog.debugLn("Hello. Mouse is at: %d %d", evt.x, evt.y);
        });

    /* Some random color generated on the separator after release? */
    BoxDividerSepWPtr boxSep = boxDivider->getSepatator(0);
    boxSep.lock()->getEvents().listen<events::LMBRelease>(
        [ref = Utils::ref<BoxDividerSep>(boxSep)](const auto&)
        {
            if (auto lockedRef = ref.lock())
            {
                lockedRef->setColor(Utils::randomRGB());
            }
        });

    /* Blocks from here on */
    app.run();

    return 0;
}