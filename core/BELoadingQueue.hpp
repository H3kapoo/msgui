#pragma once

#include <future>
#include <mutex>
#include <queue>
#include <type_traits>

#include "core/Texture.hpp"
#include "core/Window.hpp"

namespace msgui
{
using FSTextureTask = std::packaged_task<TexturePtr()>;
using UIntTask = std::packaged_task<uint32_t()>;

/* Class used to load backend resources from the main thread when loading was requested
   from secondary threads. */
class BELoadingQueue
{
public:
    static BELoadingQueue& get()
    {
        static BELoadingQueue instance;
        return instance;
    }

    void executeTasks()
    {
        while (!textureTasks_.empty())
        {
            auto& ptTex = textureTasks_.front();
            if (ptTex.valid())
            {
                ptTex();
                ptTex.reset();
            }
            textureTasks_.pop();
        }

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

    template <typename T>
    // requires (std::is_same_v<T, FSTextureTask> || std::is_same_v<T, UIntTask>)
    // requires (std::is_same_v<T, FSTextureTask>)
    requires (std::is_same_v<T, UIntTask>)
    void pushTask(T&& task)
    {
        std::unique_lock lock{mtx};

        // if (std::is_same_v<T, UIntTask>)
        {
            intTasks_.emplace(std::move(task));
        }
        // else
        // {
        //     textureTasks_.emplace(std::move(task));
        // }

        Window::requestEmptyEvent();
    }

    bool isMainThread(const uint64_t threadId) { return mainThreadId_ == threadId; }

private:
    /* Cannot be copied or moved */
    BELoadingQueue() = default;
    BELoadingQueue(const BELoadingQueue&) = delete;
    BELoadingQueue(BELoadingQueue&&) = delete;
    BELoadingQueue& operator=(const BELoadingQueue&) = delete;
    BELoadingQueue& operator=(BELoadingQueue&&) = delete;

    uint64_t mainThreadId_{std::hash<std::thread::id>{}(std::this_thread::get_id())};
    std::queue<FSTextureTask> textureTasks_;
    std::queue<UIntTask> intTasks_;
    std::mutex mtx;
};
}