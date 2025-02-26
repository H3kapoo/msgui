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
/* Channel dedicated to events coming from / going to nodes that are a logical part of other nodes. */
struct InternalChannel {};
/* Channel dedicated to events coming from / going to the user itself. */
struct UserChannel {};

class NodeEventManager
{
struct EventState
{
    bool isKeyPaused{false};
    std::function<void(INEvent&)> callback;
};

public:
    NodeEventManager() = default;

    template<typename T, typename ChannelT = UserChannel>
    requires (std::is_base_of_v<INEvent, T>)
    void listen(const std::function<void(const T)>& cb)
    {
        const auto key = computeKey<T, ChannelT>();
        eventMap_[key].callback = [cb](INEvent& evt)
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
        const auto key = computeKey<T, ChannelT>();
        if (!eventMap_.count(key)) { return; }

        if (eventMap_[key].isKeyPaused) { return; }

        eventMap_[key].callback(evt);
    }

    template<typename T>
    requires (std::is_base_of_v<INEvent, T>)
    void notifyAllChannels(T& evt)
    {
        notifyEvent<T, InputChannel>(evt);
        notifyEvent<T, InternalChannel>(evt);
        notifyEvent<T, UserChannel>(evt);
    }

    template<typename T, typename ChannelT = UserChannel>
    void pauseEvent(const bool paused = true)
    {
        const auto key = computeKey<T, ChannelT>();
        if (!eventMap_.count(key)) { return; }
        eventMap_[key].isKeyPaused = paused;
    }

    void pauseAllEvents(const bool paused = true)
    {
        for (const auto&[key, value] : eventMap_)
        {
            eventMap_[key].isKeyPaused = paused;
        }
    }

private:
    NodeEventManager(const NodeEventManager&) = delete;
    NodeEventManager(NodeEventManager&&) = delete;
    NodeEventManager& operator=(const NodeEventManager&) = delete;
    NodeEventManager& operator=(NodeEventManager&&) = delete;

    template<typename T, typename ChannelT>
    uint32_t computeKey()
    {
        std::size_t hash1 = std::type_index(typeid(T)).hash_code();
        std::size_t hash2 = std::type_index(typeid(ChannelT)).hash_code();

        return static_cast<int>(hash1 * 31 + hash2);
    }

private:
    Logger log_{"NodeEventManager"};
    std::map<uint32_t, EventState> eventMap_;
};
using NodeEventManagerPtr = std::shared_ptr<NodeEventManager>;
}; // namespace msgui::nodeevent