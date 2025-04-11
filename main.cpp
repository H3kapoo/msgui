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
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({true, true})
        .setAlignChild(Layout::Align::CENTER);

    BoxPtr b = Utils::make<Box>("mbox");
    b->getLayout().setScale({200, 300});

{    SliderPtr sl = rootBox->getHBar().lock();
    sl->getLayout().setBorderRadius({5});
    if (auto knob = sl->getKnob().lock())
    {
        knob->setBorderColor(Utils::randomRGB());
        knob->getLayout()
            .setBorderRadius({5})
            .setBorder({5})
            ;
    }}

    {    SliderPtr sl = rootBox->getVBar().lock();
    sl->getLayout().setBorderRadius({5});

        if (auto knob = sl->getKnob().lock())
        {
            knob->setBorderColor(Utils::randomRGB());
            knob->getLayout()
                .setBorderRadius({5})
                .setBorder({5})
                ;
        }}

    BoxPtr c = Utils::make<Box>("mbox");
    c->getLayout().setScale({1500, 400});
    c->setColor(Utils::randomRGB());

    rootBox->appendMany({b, c});
    // rootBox->append(lbl);

    /* Blocks from here on */
    app.run();
    return 0;
}
