#include "BELoadingQueue.hpp"

#include "msgui/Window.hpp"

namespace msgui::loaders
{
BELoadingQueue& BELoadingQueue::get()
{
    static BELoadingQueue instance;
    return instance;
}

void BELoadingQueue::executeTasks()
{
    while (!uintTasks_.empty())
    {
        auto& task = uintTasks_.front();
        if (task.valid())
        {
            task();
            task.reset();
        }
        uintTasks_.pop();
    }

    while (!fontTasks_.empty())
    {
        auto& task = fontTasks_.front();
        if (task.valid())
        {
            task();
            task.reset();
        }
        fontTasks_.pop();
    }
}

void BELoadingQueue::pushTask(UIntTask&& task)
{
    std::unique_lock lock{mtx_};
    uintTasks_.emplace(std::move(task));

    /* We need to notify main thread to run it's UI loop */
    Window::requestEmptyEvent();
}

void BELoadingQueue::pushTask(FontTask&& task)
{
    std::unique_lock lock{mtx_};
    fontTasks_.emplace(std::move(task));

    /* We need to notify main thread to run it's UI loop */
    Window::requestEmptyEvent();
}

bool BELoadingQueue::isThisMainThread()
{
    return mainThreadId_ == std::hash<std::thread::id>{}(std::this_thread::get_id());
}
} // namespace msgui::loaders