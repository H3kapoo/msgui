#include "msgui/Application.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates the use of dropdowns.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAlignChild(Layout::Align::CENTER)
        .setType(Layout::Type::HORIZONTAL);
    
    DropdownPtr dropdown = Utils::make<Dropdown>("MyDropdown");
    rootBox->append(dropdown);

    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = dropdown->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB());
            lockedBtn->setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2));
        }
    }

    DropdownWPtr subMenu = dropdown->createSubMenuItem();
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = subMenu.lock()->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB());
            lockedBtn->setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2));
        }
    }

    DropdownWPtr subMenu2 = dropdown->createSubMenuItem();
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = subMenu2.lock()->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB());
            lockedBtn->setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2));
        }
    }

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}