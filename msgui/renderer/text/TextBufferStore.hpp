#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "msgui/Logger.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui::renderer::text
{
class TextBufferStore
{
public:
    static TextBufferStore& get();

    TextDataListIt add(TextData&& data);
    bool remove(const TextDataListIt& dataIt);
    TextDataList& buffer();

private:
    /* Cannot be copied or moved */
    TextBufferStore() = default;
    TextBufferStore(const TextBufferStore&) = delete;
    TextBufferStore(TextBufferStore&&) = delete;
    TextBufferStore& operator=(const TextBufferStore&) = delete;
    TextBufferStore& operator=(TextBufferStore&&) = delete;

    Logger log_{"TextBufferStore"};

    TextDataList buffer_;
};
} // namespace msgui::renderer::text