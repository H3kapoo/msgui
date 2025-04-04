#include "msgui/renderer/text/TextBufferStore.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui::renderer::text
{
TextBufferStore& TextBufferStore::get()
{
    static TextBufferStore instance;
    return instance;
}

TextDataListIt TextBufferStore::newLocation()
{
    log_.infoLn("Requested new location");
    buffer_.emplace_back();
    return std::prev(buffer_.end());
}

bool TextBufferStore::remove(MaybeTextDataIt& maybeDataIt)
{
    if (!maybeDataIt) { return false; }

    log_.infoLn("Trying to removed from store: \"%s..\"", maybeDataIt.value()->text.substr(0, 10).c_str());
    
    /* Return true if something was actually removed. */
    if (maybeDataIt.value() != buffer_.end())
    {
        buffer_.erase(maybeDataIt.value());
        maybeDataIt = std::nullopt;
        log_.infoLn("Removed. New buffer size: %lu", buffer_.size());
        return true;
    }

    return false;
}

TextDataList& TextBufferStore::buffer() { return buffer_; }
} // namespace msgui::renderer::text