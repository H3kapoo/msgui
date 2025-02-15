#include "core/Application.hpp"
#include "core/Utils.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/Button.hpp"
#include "core/node/WindowFrame.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/nodeEvent/LMBRelease.hpp"
#include "core/ShaderLoader.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates use of buttons inside a box. Pretty simple.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setAllowOverflow({true, true})
        // .setPadding({40})
        .setAlignChild(Layout::Align::CENTER)
        // .setAlignChild({Layout::Align::RIGHT, Layout::Align::CENTER})
        // .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
        .setType(Layout::Type::HORIZONTAL);

    rootBox->getEvents().listen<LMBRelease>(
        [ref = Utils::ref<Box>(rootBox)](const auto&)
        {
            if (auto r = ref.lock())
            {
                if (r->getLayout().alignChild.y == Layout::Align::CENTER)
                {
                    r->getLayout().setAlignChild({Layout::Align::LEFT, Layout::Align::TOP});
                }
                else
                {
                    r->getLayout().
                        setAlignChild({Layout::Align::RIGHT, Layout::Align::CENTER});
                }
            }
        });

    AbstractNodePVec childBoxes;
    for (int32_t i = 0; i < 4; i++)
    {
        ButtonPtr btn = Utils::make<Button>("MyButton" + std::to_string(i));

        /* Elements are self-aligned based on the center line formed by the vertical scale of the button nodes
           and this also includes their vertical margins. Same can be said about vertically arranged buttons
           but obviously with self aligns flipped. */
        btn->getLayout()
            .setScaleType(Layout::ScaleType::ABS)
            // .setScale({84*3, 34*3});
            .setScale({70, 34});
        if (i == 0)
        {
            btn->getLayout()
                .setAlignSelf(Layout::Align::TOP)
                .setMargin({10, 0});
        }
        else if (i == 1)
        {
            btn->getLayout()
                .setAlignSelf(Layout::Align::CENTER)
                .setMargin({20, 0});
        }
        else if (i == 2)
        {
            btn->getLayout()
                .setAlignSelf(Layout::Align::BOTTOM)
                .setMargin({10, 0});
        }

        childBoxes.emplace_back(btn);
    }
    rootBox->appendMany(childBoxes);

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}