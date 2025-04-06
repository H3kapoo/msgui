#include "msgui/loaders/FontLoader.hpp"

#include <future>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "msgui/loaders/BELoadingQueue.hpp"
#include "msgui/Logger.hpp"
#include "msgui/renderer/Types.hpp"
#include "msgui/vendor/stb_image_write.h"

namespace msgui::loaders
{
std::unordered_map<std::string, std::shared_ptr<Font>> FontLoader::fontPathToObject_ = {};

FontLoader& FontLoader::get()
{
    static FontLoader instance;
    return instance;
}

FontLoader::FontLoader()
{
    if (FT_Init_FreeType(&ftLib_))
    {
        log_.errorLn("FreeType lib failed to load!");
    }
}

FontLoader::~FontLoader()
{
    FT_Done_FreeType(ftLib_);
    log_.debugLn("Deallocated.");
}

FontPtr FontLoader::loadFont(const std::string& fontPath, const int32_t fontSize)
{
    std::string fontKey = fontPath + std::to_string(fontSize);
    if (fontPathToObject_.count(fontKey))
    {
        return fontPathToObject_.at(fontKey);
    }

    std::packaged_task<FontPtr()> task([this, fontPath, fontSize]()
    {
        return loadFontInternal(fontPath, fontSize);
    });

    auto futureTask = task.get_future();

    if (BELoadingQueue::get().isThisMainThread()) { task(); }
    /* This is not the main thread */
    else { BELoadingQueue::get().pushTask(std::move(task)); }

    fontPathToObject_[fontKey] = futureTask.get();

    return fontPathToObject_.at(fontKey);
}

FontPtr FontLoader::loadFontInternal(const std::string& fontPath, const int32_t fontSize)
{
    FontPtr font = std::make_shared<Font>();
    font->fontSize = fontSize;
    font->fontPath = fontPath;

    if (fontSize < MIN_FONT_SIZE || fontSize > MAX_FONT_SIZE)
    {
        log_.errorLn("Failed to load font: \"%s\". Size is out of bounds: %d. Will keep previous font size.",
            fontPath.c_str(), fontSize);
        return font;
    }

    FT_Face ftFace;
    if (FT_New_Face(ftLib_, fontPath.c_str(), 0, &ftFace))
    {
        log_.errorLn("Failed to load font: \"%s\". Will keep previous font.", fontPath.c_str());
        return font;
    }

    FT_Set_Pixel_Sizes(ftFace, fontSize, fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &font->texId);

    if (!font->texId)
    {
        log_.warnLn(
            "No failures but font texture id is zero. Are you loading incorrectly from another openGL context?");
        return font;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font->texId);

    /* Generate MAX_CODEPOINTS levels deep texture. */
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, fontSize, fontSize, MAX_CODEPOINTS, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    /* Wrapping, mag & min settings. */
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FT_Int32 load_flags = FT_LOAD_RENDER;
    for (int32_t i = 32; i < MAX_CODEPOINTS; i++)
    {
        /* Loads i'th char in font atlas */
        if (FT_Load_Char(ftFace, i, load_flags))
        {
            log_.errorLn("Error loading char code: %d", i);
            continue;
        }

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, 1,
            GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);

        Font::CodePointData ch =
        {
            .charCode = uint32_t(i),
            .hAdvance = ftFace->glyph->advance.x,
            .size = glm::ivec2(ftFace->glyph->bitmap_left + ftFace->glyph->bitmap.width,
                ftFace->glyph->bitmap_top + ftFace->glyph->bitmap.rows),
            .bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top)
        };

        font->codePointData[i] = ch;
    }

    log_.infoLn("Loaded font texture {%d} with size %d from \"%s\"", font->texId, fontSize, fontPath.c_str());

    /* Unbind texture and free FreeType resources */
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    FT_Done_Face(ftFace);

    return font;
}
} // namespace msgui::loaders