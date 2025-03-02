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
        .setAlignChild(Layout::Align::CENTER);

    SliderPtr slider = Utils::make<Slider>("MySlider");
    slider->getLayout()
        .setBorder({1})
        .setType(Layout::Type::VERTICAL)
        .setScale({20, 200});
    slider->setSlideFrom(20)
        .setSlideTo(100)
        .setBorderColor(Utils::hexToVec4("#333333"));

    /* You can also change the size of the knob itself, the Y value in case of a vertical slider.
       Note the use of direct assignment instead of setScale(..). Setter functions can execute layout
       refresh operations, but we don't need those here as the app hasn't even started yet (the run() call)
       so we can just directly assign the value we need to change. */
    slider->getKnob().lock()->getLayout().scale.y = 10;

    /* Dynamically change the size of the slider as you scroll it. */
    slider->getEvents().listen<nodeevent::Scroll>(
        [ref = Utils::ref<Slider>(slider)](const auto& evt)
        {
            ref.lock()->getLayout().setScale(
            {
                (int32_t)evt.value,
                ref.lock()->getLayout().scale.y
            });
        });

    rootBox->append(slider);

    /* Blocks from here on */
    app.run();

    return 0;
}