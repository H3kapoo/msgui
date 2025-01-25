#include "core/Application.hpp"
#include "core/Utils.hpp"
#include "core/node/Box.hpp"
#include "core/node/WindowFrame.hpp"

using namespace msgui;

int main()
{
    Application& app = Application::get();
    if (!app.init()) { return 1; }

    WindowFramePtr window = app.createFrame("MainWindow", 1280, 720);

    BoxPtr rootBox = window->getRoot();
    rootBox->setColor(Utils::hexToVec4("#4aabebff"));

    app.setPollMode(Application::PollMode::ON_EVENT);
    app.setVSync(Application::Toggle::ON);

    app.run();

    return 0;
}