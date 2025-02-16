#pragma once

#include <memory>
#include <functional>
#include <map>
#include <type_traits>
#include <typeindex>

#include "msgui/nodeEvent/INodeEvent.hpp"
#include "msgui/Logger.hpp"

namespace msgui::nodeevent
{
/* Channels overdered by priority and order of fire. */
/* Channel dedicated to events coming from the window manager directly to the nodes. */
struct InputChannel {};
/* Channel dedicated to events coming from nodes that are a logical part of other nodes. */
struct InternalChannel {};
/* Channel dedicated to events coming from the user itself. */
struct UserChannel {};

class NodeEventManager
{
public:
    NodeEventManager() = default;

    template<typename T, typename ChannelT>
    uint32_t computeKey()
    {
        std::size_t hash1 = std::type_index(typeid(T)).hash_code();
        std::size_t hash2 = std::type_index(typeid(ChannelT)).hash_code();

        return static_cast<int>(hash1 * 31 + hash2);
    }

    template<typename T, typename ChannelT = UserChannel>
    requires (std::is_base_of_v<INEvent, T>)
    void listen(const std::function<void(const T)>& cb)
    {
        eventMap_[computeKey<T, ChannelT>()] = [cb](INEvent& evt)
        {
            if (const auto derived = dynamic_cast<T*>(&evt))
            {
                cb(*derived);
            }
        };
    }

    template<typename T, typename ChannelT = UserChannel>
    requires (std::is_base_of_v<INEvent, T>)
    void ignore()
    {
        eventMap_.erase(computeKey<T, ChannelT>());
    }

    template<typename T, typename ChannelT = UserChannel>
    requires (std::is_base_of_v<INEvent, T>)
    void notifyEvent(T& evt)
    {
        if (paused_) { return; }

        auto key = computeKey<T, ChannelT>();
        if (!eventMap_.count(key)) { return; }

        eventMap_[key](evt);
    }

    template<typename T>
    requires (std::is_base_of_v<INEvent, T>)
    void notifyAllChannels(T& evt)
    {
        notifyEvent<T, InputChannel>(evt);
        notifyEvent<T, InternalChannel>(evt);
        notifyEvent<T, UserChannel>(evt);
    }

    void pauseAll(const bool paused = true)
    {
        paused_ = paused;
    }

private:
    NodeEventManager(const NodeEventManager&) = delete;
    NodeEventManager(NodeEventManager&&) = delete;
    NodeEventManager& operator=(const NodeEventManager&) = delete;
    NodeEventManager& operator=(NodeEventManager&&) = delete;

private:
    Logger log_{"NodeEventManager"};
    bool paused_{false};
    std::map<uint32_t, std::function<void(INEvent&)>> eventMap_;
};
using NodeEventManagerPtr = std::shared_ptr<NodeEventManager>;
}; // namespace msgui::nodeevent