#include "core/Application.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    /*
        Showcases how a Box can be scaled and placed inside another Box container,
        how the children inside the container can be aligned, how the children themselves
        can align based on the rest of the children on the line, margins, colors, padding.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setPadding({0, 10, 40, 40})
        .setAlignChild({Layout::Align::CENTER, Layout::Align::BOTTOM})
        .setType(Layout::Type::HORIZONTAL);

    AbstractNodePVec childBoxes;
    for (int32_t i = 0; i < 4; i++)
    {
        BoxPtr box = Utils::make<Box>("MyBoxName" + std::to_string(i));
        box->setColor(Utils::randomRGB());
        box->getLayout()
            .setType(Layout::Type::HORIZONTAL)
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setAlignSelf(Layout::Align::CENTER)
            .setScale({0.25f, 300 * Utils::random01() + 50})
            .setMargin({5, 0, 5, 5});
        childBoxes.emplace_back(box);
    }
    rootBox->appendMany(childBoxes);

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}