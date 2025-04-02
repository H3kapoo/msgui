#include "msgui/renderer/text/TextBufferStore.hpp"
#include "msgui/renderer/text/Types.hpp"
#include <optional>

namespace msgui::renderer::text
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

    log_.debugLn("Removed from store: \"%s\"", maybeDataIt.value()->text.c_str());

    /* Return true if something was actually removed. */
    if (maybeDataIt.value() != buffer_.end())
    {
        buffer_.erase(maybeDataIt.value());
        maybeDataIt = std::nullopt;
        return true;
    }

    return false;
}

TextDataList& TextBufferStore::buffer() { return buffer_; }
} // namespace msgui::renderer::text