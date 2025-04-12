#include "msgui/node/Slider.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/events/Scroll.hpp"

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
        .setAlignChild(utils::Layout::Align::CENTER);

    SliderPtr slider = Utils::make<Slider>("MySlider");
    slider->getLayout()
        .setType(utils::Layout::Type::VERTICAL)
        .setScale({40, 200});
    slider->setSlideFrom(20)
        .setSlideTo(100)
        .setBorderColor(Utils::hexToVec4("#333333"));

    /* Dynamically change the size of the slider as you scroll it. */
    slider->getEvents().listen<events::Scroll>(
        [ref = Utils::ref<Slider>(slider)](const auto& evt)
        {
            // ref.lock()->getLayout().setScale(
            // {
            //     (int32_t)evt.value,
            //     ref.lock()->getLayout().scale.y
            // });
        });

    rootBox->append(slider);

    /* Blocks from here on */
    app.run();

    return 0;
}