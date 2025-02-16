#include "BELoadingQueue.hpp"

#include "msgui/Window.hpp"

namespace msgui
{
BELoadingQueue& BELoadingQueue::get()
{
    static BELoadingQueue instance;
    return instance;
}

void BELoadingQueue::executeTasks()
{
    while (!tasks_.empty())
    {
        auto& task = tasks_.front();
        if (task.valid())
        {
            task();
            task.reset();
        }
        tasks_.pop();
    }
}

void BELoadingQueue::pushTask(UIntTask&& task)
{
    std::unique_lock lock{mtx_};
    tasks_.emplace(std::move(task));

    /* We need to notify main thread to run it's UI loop */
    Window::requestEmptyEvent();
}

bool BELoadingQueue::isMainThread(const uint64_t threadId)
{
    return mainThreadId_ == threadId;
}
} // namespace msgui