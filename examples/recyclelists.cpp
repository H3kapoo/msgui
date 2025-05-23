#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
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
    rl->setBorderColor(Utils::hexToVec4("#ffffff"));
    rl->getLayout()
        .setScaleType({Layout::ScaleType::PX, Layout::ScaleType::REL})
        .setScale({300, 0.8f})
        .setBorder({4})
        .setBorderRadius({4});

    rootBox->append(rl);

    /* A lot of things are customizable, such as: row size, items margin/border/border radius, container's
       generic props are also available as well as the slider's props. */
    rl->setRowSize(20);

    rl->getContainer().lock()->getLayout().setBorder({4});
    rl->getContainer().lock()->getLayout().setBorderRadius({4});
    rl->getContainer().lock()->setBorderColor(Utils::hexToVec4("#bebebe"));

    rl->getSlider().lock()->getLayout().setScale({20, 1.0f});
    rl->getSlider().lock()->getLayout().setBorderRadius({5});

    rl->getSlider().lock()->getKnob().lock()->getLayout().setBorder({0});
    rl->getSlider().lock()->getKnob().lock()->getLayout().setBorderRadius({5});

    /* Basic node addition. */
    for (int32_t i = 0; i < 30; i++)
    {
        rl->addItem(Utils::randomRGB());
    }

    /* Do custom logic when the user clicks (technically releases click) on an item. */
    rl->getEvents().listen<nodeevent::LMBItemRelease>(
        [mainLogger, ref = Utils::ref<RecycleList>(rl)](const auto& evt)
        {
            (void)evt;
            /* User can add items. (uncomment bellow) */
            ref.lock()->addItem(Utils::randomRGB());
            /* User can remove items by index. (uncomment bellow)*/
            // ref.lock()->removeItemIdx(evt.index);
            /* User can remove items by a predicate. (uncomment bellow) */
            // ref.lock()->removeItemsBy([](const glm::vec4& item) -> bool
            // {
            //     return item.r > 0.5f;
            // });
        });

    /* Blocks from here on */
    app.run();

    return 0;
}