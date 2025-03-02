#include "msgui/node/Slider.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/Scroll.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates the use of sliders. These can display a range of values the user can pick from.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
        .setAlignChild(Layout::Align::CENTER);
        // .setAlignChild(Layout::Align::LEFT);

    SliderPtr slider = Utils::make<Slider>("MySlider");
    slider->getLayout()
        // .setBorder({2, 2, 2, 2})
        .setBorder({1})
        .setType(Layout::Type::VERTICAL)
        .setScale({35, 200});
    slider->setSlideFrom(20)
        .setSlideTo(100)
        .setBorderColor(Utils::hexToVec4("#333333"))
        ;

    // slider->getEvents().listen<nodeevent::Scroll>(
    //     [ref = Utils::ref<Slider>(slider)](const auto& evt)
    //     {
    //         ref.lock()->setGirth(evt.value);
    //     });
    rootBox->append(slider);
    // rootBox->append(newBox);
    /* Blocks from here on */
    app.run();

    return 0;
}