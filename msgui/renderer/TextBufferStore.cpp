#include "TextBufferStore.hpp"
#include "msgui/renderer/Types.hpp"

namespace msgui::renderer
{
TextBufferStore& TextBufferStore::get()
{
    static TextBufferStore instance;
    return instance;
}

TextDataListIt TextBufferStore::newLocation()
{
    log_.debugLn("Requested new location");
    buffer_.emplace_back();
    return std::prev(buffer_.end());
}

bool TextBufferStore::remove(MaybeTextDataIt& maybeDataIt)
{
    if (!maybeDataIt) { return false; }

    /* Return true if something was actually removed. */
    if (maybeDataIt.value() != buffer_.end())
    {
        buffer_.erase(maybeDataIt.value());
        maybeDataIt = std::nullopt;
        log_.debugLn("Removed. New buffer size: %lu", buffer_.size());
        return true;
    }

    return false;
}

TextDataList& TextBufferStore::buffer() { return buffer_; }
} // namespace msgui::renderer