#include "core/Application.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    /*
        Same as boxes.cpp but with wrapping functionality.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAllowOverflow({false, false}) /* NO need to allow overflow if we gonna wrap. */
        .setAllowWrap(true)
        .setType(Layout::Type::HORIZONTAL);

    AbstractNodePVec childBoxes;
    for (int32_t i = 0; i < 10; i++)
    {
        BoxPtr box = Utils::make<Box>("MyBoxName" + std::to_string(i));
        box->setColor(Utils::randomRGB());
        box->getLayout()
            .setType(Layout::Type::HORIZONTAL)
            .setScaleType(Layout::ScaleType::ABS) /* Be aware that wrap + rel doesn't work pretty nicely. */
            .setAlignSelf(Layout::Align::CENTER)
            .setScale({200 * Utils::random01() + 50, Utils::random01() * 200 + 50})
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