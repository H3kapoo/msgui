#pragma once

#include <future>
#include <mutex>
#include <queue>
#include <functional>

#include "msgui/renderer/Types.hpp"
#include "msgui/common/Defines.hpp"

namespace msgui::loaders
{

using UIntTask = std::packaged_task<uint32_t()>;
using FontTask = std::packaged_task<renderer::FontPtr()>;

/* Class used to load backend resources from the main thread when loading was requested
   from secondary threads.
   Note: Currently meshes haven't been included into this as we only use quads for now.
         As long as the main window is created in the main thread, it all shall go well.
*/
class BELoadingQueue
{
public:
    /**
        Get instance of this.

        @return Instance of this
    */
    static BELoadingQueue& get();

    /**
        Execute all pushed tasks in the main UI thread.
    */
    void executeTasks();

    /**
        Push task that will resolve an unsigned integer return task.
        
        @param task Task function to be executed
    */
    void pushTask(UIntTask&& task);

    /**
        Push task that will resolve a FontPtr return task.

        @param task Task function to be executed
    */
    void pushTask(FontTask&& task);

    /**
        Check if the calling thread is the main UI one.

        @return True is the thread is the main one
    */
    bool isThisMainThread();

private:
    /* Cannot be copied or moved */
    BELoadingQueue() = default;
    BELoadingQueue(const BELoadingQueue&) = delete;
    BELoadingQueue(BELoadingQueue&&) = delete;
    BELoadingQueue& operator=(const BELoadingQueue&) = delete;
    BELoadingQueue& operator=(BELoadingQueue&&) = delete;

    uint64_t mainThreadId_{std::hash<std::thread::id>{}(std::this_thread::get_id())};
    std::queue<UIntTask> uintTasks_;  // TODO: Ideally we shall have a single queue
    std::queue<FontTask> fontTasks_;
    std::mutex mtx_;
};
} // namespace msgui::loaders