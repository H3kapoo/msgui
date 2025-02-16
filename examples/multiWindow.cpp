#include "msgui/Application.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr& window = app.createFrame("MainWindow", 1280, 720);
    WindowFramePtr& window2 = app.createFrame("MainWindow2", 800, 600);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));

    BoxPtr rootBox2 = window2->getRoot();
    rootBox2->setColor(Utils::hexToVec4("#4aabebff"));

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(true);

    /* Blocks from here on */
    app.run();

    return 0;
}