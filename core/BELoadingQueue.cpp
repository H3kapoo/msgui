#include "BELoadingQueue.hpp"

#include "core/Window.hpp"

namespace msgui
{
BELoadingQueue& BELoadingQueue::get()
{
    static BELoadingQueue instance;
    return instance;
}

void BELoadingQueue::executeTasks()
{
    while (!intTasks_.empty())
    {
        auto& task = intTasks_.front();
        if (task.valid())
        {
            task();
            task.reset();
        }
        intTasks_.pop();
    }
}

void BELoadingQueue::pushTask(UIntTask&& task)
{
    std::unique_lock lock{mtx};
    intTasks_.emplace(std::move(task));
    Window::requestEmptyEvent();
}

bool BELoadingQueue::isMainThread(const uint64_t threadId)
{
    return mainThreadId_ == threadId;
}
} // namespace msgui