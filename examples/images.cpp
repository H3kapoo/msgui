#include "msgui/Application.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    /*
        Demonstrates the use of the image node.
    */
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));
    rootBox->getLayout()
        .setSpacing(Layout::Spacing::EVEN_WITH_START_GAP)
        .setAlignChild(Layout::Align::CENTER);

    AbstractNodePVec childImages;
    for (int32_t i = 0; i < 4; i++)
    {
        ImagePtr img = Utils::make<Image>("MyImageName" + std::to_string(i));
        img->setImage(i % 2 ? "assets/textures/wall.jpg" : "assets/textures/container.jpg");
        img->setTint(Utils::randomRGB());
        img->getLayout()
            .setType(Layout::Type::HORIZONTAL)
            .setScaleType(Layout::ScaleType::ABS)
            .setAlignSelf(Layout::Align::CENTER)
            .setScale({200, 200})
            .setBorder({4})
            .setBorderRadius({6});
            ;
        childImages.emplace_back(img);
    }

    rootBox->appendMany(childImages);

    /* Blocks from here on */
    app.run();

    return 0;
}