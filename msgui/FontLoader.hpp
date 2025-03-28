#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "vendor/stb_image_write.h"

#include "msgui/Logger.hpp"

namespace msgui
{
/* Class that loads a font and stores it uniquely accross windows */
class FontLoader
{
    #define MAX_ASCII 128
    // #define MAX_ASCII 256

    struct ASCIIChar
    {
        uint32_t charCode;
        int64_t hAdvance;
        glm::ivec2 size;
        glm::ivec2 bearing;
    };

    struct LoadedFont
    {
        ASCIIChar data[MAX_ASCII];
        uint32_t texId;
    };

public:
    uint32_t loadFont(const std::string& fontPath, const int32_t fontSize)
    {
        /*
            Note: there are still problems with loading SDF fonts and sending them to the GPU.
            Some glyphs do reach the gpu, some dont. If we increase the fontSize, more glpyhs can
            reach the gpu and it's very strange and unintuitive. Will tackle this problem in the future.
            For now basic bitmap fonts are enough.
            Alternatively: Use premade SDF font atlas.
        */

        FT_Library ftLib;
        if (FT_Init_FreeType(&ftLib))
        {
            log_.errorLn("FreeType lib failed to load!");
            // return nullptr;
            return 0;
        }

        FT_Face ftFace;
        if (auto e = FT_New_Face(ftLib, fontPath.c_str(), 0, &ftFace))
        {
            log_.errorLn("Failed to load font: %s, error %d", fontPath.c_str(), e);
            // return nullptr;
            return 0;
        }

        std::string fontKey = fontPath + std::to_string(fontSize);
        // TODO: To avoid one copy of LoadedFont, pointers can be used. For now its ok
        LoadedFont font;

        const int32_t charLimit = MAX_ASCII;
        FT_Set_Pixel_Sizes(ftFace, fontSize, fontSize);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &font.texId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, font.texId);

        // generate 128 lvls texture
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, fontSize*2, fontSize*2, charLimit, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        // wrapping & mag settings
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        FT_Int32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
        // FT_GlyphSlot slot = ftFace->glyph; //SDF
        for (int32_t i = 32; i < 128; i++)
        {
            /* Loads i'th char in font atlas */
            if (FT_Load_Char(ftFace, i, load_flags))
            {
                log_.errorLn("Error loading char code: %d", i);
                continue;
            }
            // FT_Render_Glyph(slot, FT_RENDER_MODE_SDF); // SDF

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, 1,
                GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);

            ASCIIChar ch = {.charCode = uint32_t(i),
                .hAdvance = ftFace->glyph->advance.x,
                .size = glm::ivec2(ftFace->glyph->bitmap_left + ftFace->glyph->bitmap.width,
                    ftFace->glyph->bitmap_top + ftFace->glyph->bitmap.rows),
                .bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top)};

            font.data[i] = ch;
        }

        // gFontPathToGenFont[fontKey] = font;

        log_.infoLn("Loaded font texture {%d} from %s", font.texId, fontPath.c_str());

        // unbind texture and free FTLIB resources
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLib);

        return font.texId;
        // return &gFontPathToGenFont[fontKey];
    }

private:
    Logger log_{"FontLoader"};
};
} // namespace msgui