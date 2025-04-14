#include "msgui/Application.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/WindowFrame.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/events/LMBRelease.hpp"

using namespace msgui;
using namespace msgui::layoutengine::utils;

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

    /* Dropdowns can have a preffered expand direction. */
    DropdownPtr dropdown = Utils::make<Dropdown>("MyDropdown");
    dropdown->getLayout()
        .setScale({100, 30})
        .setBorder({1});
    dropdown->setItemSize({170, 30})
        .setExpandDirection(Dropdown::Expand::BOTTOM)
        .setBorderColor(Utils::hexToVec4("#000000ff"));
        rootBox->append(dropdown);

    /* Create some menu items. */
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = dropdown->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB())
                .setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2))
                .setBorderColor(Utils::hexToVec4("#000000ff"));
            lockedBtn->getLayout().setBorder({1}).setMargin({0, 1, 0, 0});
        }
    }

    /* Create a new dropdown submenu inside the main dropdown. */
    DropdownWPtr subMenu = dropdown->createSubMenuItem();
    subMenu.lock()->getLayout().setBorder({1});
    subMenu.lock()->setExpandDirection(Dropdown::Expand::RIGHT)
        .setColor(Utils::hexToVec4("#535353ff"))
        .setBorderColor(Utils::hexToVec4("#000000ff"));
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = subMenu.lock()->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB())
                .setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2))
                .setBorderColor(Utils::hexToVec4("#000000ff"));
            lockedBtn->getLayout().setBorder({1}).setMargin({0, 1, 0, 0});

            /* On mouse release, change submenu items to have a new size and also remove the first item
               from the dropdown. */
            lockedBtn->getEvents().listen<events::LMBRelease>([subMenu](const auto&)
            {
                if (auto locked = subMenu.lock())
                {
                    locked->setItemSize({170, 30});
                    locked->removeMenuItemIdx(0);
                }
            });
        }
    }

    /* Another dropdown that opens to the left by default. Also disable the item on click. */
    DropdownWPtr subMenu2 = dropdown->createSubMenuItem();
    subMenu2.lock()->getLayout()
        .setBorder({1})
        .setMargin({1, 0, 0, 0});
    subMenu2.lock()->setExpandDirection(Dropdown::Expand::LEFT)
        .setColor(Utils::hexToVec4("#333333ff"))
        .setBorderColor(Utils::hexToVec4("#000000ff"));
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonWPtr btn = subMenu2.lock()->createMenuItem<Button>();
        if (auto lockedBtn = btn.lock())
        {
            lockedBtn->setColor(Utils::randomRGB());
            lockedBtn->setPressedColor(Utils::darken(lockedBtn->getColor(), 0.2));
            lockedBtn->getEvents().listen<events::LMBRelease>(
                [ref = Utils::ref<Button>(lockedBtn)](const auto&)
                {
                    ref.lock()->setEnabled(false);
                });
        }
    }

    /* Blocks from here on */
    app.run();

    return 0;
}