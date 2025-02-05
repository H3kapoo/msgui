#pragma once

#include <future>
#include <mutex>
#include <queue>

namespace msgui
{
using UIntTask = std::packaged_task<uint32_t()>;

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
        Push task that will return unsigned integer as a promise.

        @param task Task function to be executed
    */
    void pushTask(UIntTask&& task);

    /**
        Check if the calling thread is the main UI one.

        @return True is the thread is the main one
    */
    bool isMainThread(const uint64_t threadId);

private:
    /* Cannot be copied or moved */
    BELoadingQueue() = default;
    BELoadingQueue(const BELoadingQueue&) = delete;
    BELoadingQueue(BELoadingQueue&&) = delete;
    BELoadingQueue& operator=(const BELoadingQueue&) = delete;
    BELoadingQueue& operator=(BELoadingQueue&&) = delete;

    uint64_t mainThreadId_{std::hash<std::thread::id>{}(std::this_thread::get_id())};
    std::queue<UIntTask> intTasks_;
    std::mutex mtx;
};
} // namespace msgui