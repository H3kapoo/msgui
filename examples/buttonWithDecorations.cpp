#include <string>

#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;

int main()
{
    /*
        Use of Button's image and text attach feature.
    */

    Application& app = Application::get();
    if (!app.init()) { return 1; }

    Logger mainLogger("mainLog");

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#252525ff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER);

    ButtonPtr btn = Utils::make<Button>("mybutton");
    btn->setColor(Utils::hexToVec4("#d33f3fff"))
        .setImagePath("assets/textures/awesomeface.png")
        .setText("Hello click me!");

    btn->getLayout()
        .setScale({200, 50});

    TextLabelPtr lbl = btn->getTextLabel().lock();
    lbl->setFontSize(20);

    ImagePtr img = btn->getImage().lock();
    img->getLayout().setScale({46, 46}).setMargin({2});

    rootBox->append(btn);

    /* Blocks from here on */
    app.run();
    return 0;
}
