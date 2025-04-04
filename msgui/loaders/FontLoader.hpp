#pragma once

#include <future>
#include <unordered_map>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "msgui/vendor/stb_image_write.h"
#include "msgui/Logger.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui::loaders
{
using namespace renderer::text;

/* Class that loads a font and stores it uniquely accross windows */
class FontLoader
{
public:
    static FontLoader& get();

    FontPtr loadFont(const std::string& fontPath, const int32_t fontSize = DEFAULT_FONT_SIZE);
    
private:
    FontLoader();
    ~FontLoader();

    FontPtr loadFontInternal(const std::string& fontPath, const int32_t fontSize);

    /* Cannot be copied or moved */
    FontLoader(const FontLoader&) = delete;
    FontLoader(FontLoader&&) = delete;
    FontLoader& operator=(const FontLoader&) = delete;
    FontLoader& operator=(FontLoader&&) = delete;

private:
    Logger log_{"FontLoader"};
    FT_Library ftLib_;

    static std::unordered_map<std::string, FontPtr> fontPathToObject_;
};
} // namespace msgui::loaders