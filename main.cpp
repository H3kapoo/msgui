#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/Application.hpp"
#include "msgui/Logger.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/node/utils/LayoutData.hpp"
#include "msgui/nodeEvent/FocusLost.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/RMBRelease.hpp"
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
        .setBorder({1})
        .setType(Layout::Type::VERTICAL)
        .setScale({35, 200});
    slider->setSlideFrom(20)
        .setSlideTo(100)
        .setBorderColor(Utils::hexToVec4("#333333"))
        ;

    for (int i = 0; i < 4; i++)
    {
        BoxPtr box = Utils::make<Box>("newBox");
        box->getLayout().setScale({200, 200});
        box->setColor(Utils::randomRGB());
        rootBox->append(box);
    }

    DropdownPtr dd = Utils::make<Dropdown>("dd");
    dd->setColor(Utils::randomRGB());
    for (int i = 0; i < 4; i++)
    {
        auto btn = dd->createMenuItem<Button>();
        btn.lock()->setColor(Utils::randomRGB());
    }

    DropdownPtr dd2 = Utils::make<Dropdown>("dd2");
    dd2->setColor(Utils::randomRGB());
    for (int i = 0; i < 4; i++)
    {
        auto btn = dd2->createMenuItem<Button>();
        btn.lock()->setColor(Utils::randomRGB());
    }

    DropdownPtr dd3 = Utils::make<Dropdown>("dd3");
    // dd3->setExpandDirection(Dropdown::Expand::RIGHT);
    dd3->setExpandDirection(Dropdown::Expand::TOP);
    // dd3->getContainer().lock()->getLayout().setPadding({4, 4, 4, 4});
    dd3->getContainer().lock()->setColor(Utils::randomRGB());
    // dd3->getLayout().setBorder({4});
    // dd3->setColor(Utils::randomRGB())
    //     .setBorderColor(Utils::hexToVec4("#000000ff"));
    for (int i = 0; i < 8; i++)
    {
        auto btn = dd3->createMenuItem<Button>();
        btn.lock()->getLayout().setScale({100, 34});
        btn.lock()->setColor(Utils::randomRGB());
        btn.lock()->getLayout().setBorder({6, 6});
        // btn.lock()->getLayout().setBorder({4});
    }

    // rootBox->setContextMenu(dd);
    // Utils::as<Box>(rootBox->getChildren()[0])->setContextMenu(dd2);
    Utils::as<Box>(rootBox->getChildren()[2])->setContextMenu(dd3);

    // rootBox->printTree();

    /* Blocks from here on */
    app.run();

    return 0;
}