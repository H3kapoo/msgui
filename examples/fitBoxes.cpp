#include "msgui/Application.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"

using namespace msgui;

int main()
{
    /*
        Showcases how use the FIT ScaleType.
        Note: - If the parent is ScaleType::FIT then the children cannot be ScaleType::REL on the
            parent's ScaleType::FIT axis.
              - If the parent is ScaleType::FIT and the child X doesn't have any children then
            child X must have ScaleType::PX on the parent's ScaleType::FIT axis.
              - If the parent is ScaleType::FIT then the children can be ScaleType::FIT on the
            parent's ScaleType::FIT axis.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout().setAlignChild(Layout::Align::CENTER)
        // .setScale({1280, 720})
    ;

    BoxPtr boxMain = Utils::make<Box>("NewBox");
    boxMain->getLayout()
        .setScale({300, 300})
        .setScaleType(Layout::ScaleType::PX)
        ;
    rootBox->append(boxMain);

    AbstractNodePVec childBoxes;
    for (int32_t i = 0; i < 4; i++)
    {
        BoxPtr box = Utils::make<Box>("MyBoxName" + std::to_string(i));
        box->setColor(Utils::randomRGB());
        box->getLayout()
            .setScaleType(Layout::ScaleType::PX)
            .setScale({100, 100})
            // .setMargin({5, 0, 5, 5})
            ;
        childBoxes.emplace_back(box);
    }
    boxMain->appendMany(childBoxes);

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}