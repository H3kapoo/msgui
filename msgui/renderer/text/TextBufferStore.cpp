#include "msgui/renderer/text/TextBufferStore.hpp"

namespace msgui::renderer::text
{
TextBufferStore& TextBufferStore::get()
{
    static TextBufferStore instance;
    return instance;
}

TextDataListIt TextBufferStore::add(TextData&& data)
{
    /* Using list here instead of vector presevers the iterators validity regardless of if we add/remove
       elements to the list. Using vectors would mean possible reallocations and iterators getting invalidated.
       Returning list iterators is more prefferable here for later deletions instead of std::finding each element
       to be deleted from a std::vector. That's the current assumption at least for now. */
    log_.debugLn("Added to store: \"%s\"", data.text.c_str());

    return buffer_.insert(buffer_.end(), std::move(data));
}

bool TextBufferStore::remove(const TextDataListIt& dataIt)
{
    log_.debugLn("Removed from store: \"%s\"", dataIt->text.c_str());

    /* Return true if something was actually removed. */
    if (dataIt != buffer_.end())
    {
        buffer_.erase(dataIt);
        return true;
    }

    return false;
}

TextDataList& TextBufferStore::buffer() { return buffer_; }
} // namespace msgui::renderer::text