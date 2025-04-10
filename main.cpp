#include <string>

#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/Scroll.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#252525ff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER);

    SliderPtr slider = Utils::make<Slider>("my slider");
    slider->setSlideFrom(0)
        .setSlideTo(100)
        .setSlideCurrentValue(59)
        .setSensitivity(4)
        // .enableViewValue(false)
        .setColor(Utils::hexToVec4("#ba30beff"));
        ;
    // slider->getLayout()
    //     .setType(Layout::Type::VERTICAL);
    // slider->getLayout().setBorderRadius({17}).setType(Layout::Type::VERTICAL);
    slider->getKnob().lock()->getLayout().setScale({40, 1.0f});
    slider->getLayout()
        .setScale({300, 40})
        .setMargin({0, 0, 0, 5})
        ;
    // slider->getKnob().lock()->getLayout().setScale({1.0f, 40});

    TextLabelPtr lbl = Utils::make<TextLabel>("mylbl");
    lbl->setText(std::to_string(int32_t(slider->getSlideCurrentValue())));
    lbl->getLayout().setScale({40, 40});

    slider->getEvents().listen<events::Scroll>(
        [label = Utils::ref<TextLabel>(lbl)](const auto& evt)
        {
            label.lock()->setText(std::to_string(int32_t(evt.value)));
            // ref.lock()->getLayout().setType(Layout::Type::HORIZONTAL);
        });

    rootBox->append(slider);
    rootBox->append(lbl);

    /* Blocks from here on */
    app.run();
    return 0;
}
