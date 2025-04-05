#include "TextRenderer.hpp"

#include "msgui/loaders/FontLoader.hpp"
#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/Window.hpp"
#include "msgui/renderer/TextBufferStore.hpp"
#include "msgui/renderer/Types.hpp"

namespace msgui::renderer
{
TextRenderer::TextRenderer()
{
    mesh_ = loaders::MeshLoader::loadQuad();
    shader_ = loaders::ShaderLoader::loadShader("assets/shader/textInstanced.glsl");
    fallbackFontTexId_ = loaders::FontLoader::get().loadFont(DEFAULT_FONT_PATH)->texId;

    shaderBuffer_.transform.reserve(MAX_SHADER_BUFFER_SIZE);
    shaderBuffer_.unicodeIndex.reserve(MAX_SHADER_BUFFER_SIZE);
}

void TextRenderer::render(const glm::mat4& projMat, const int32_t frameHeight)
{
    batchCount = 0;

    mesh_->bind();
    shader_->bind();
    shader_->setMat4f("uProjMat", projMat);
    shader_->setVec4f("uColor", color_);
    shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, fallbackFontTexId_);

    clearInternalBuffer();

    auto& storeBuffer = TextBufferStore::get().buffer();
    for (auto& element : storeBuffer)
    {
        doScissorMask(element.transformPtr, frameHeight);

        /* Use a fallback font in case the main one is not provided for some reason. */
        element.fontData->texId
            ? shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, element.fontData->texId)
            : shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, fallbackFontTexId_);

        int32_t copiedSize = 0;
        int32_t copyStart = 0;
        int32_t copyEnd = 0;
        while ((int32_t)element.pcd.transform.size() > copiedSize)
        {
            int32_t spaceLeft = MAX_SHADER_BUFFER_SIZE - shaderBuffer_.transform.size();
            copyEnd = copyStart + std::min(spaceLeft, (int32_t)element.pcd.transform.size());

            std::copy(element.pcd.transform.begin() + copyStart, element.pcd.transform.begin() + copyEnd,
                std::back_inserter(shaderBuffer_.transform));
            std::copy(element.pcd.unicodeIndex.begin() + copyStart, element.pcd.unicodeIndex.begin() + copyEnd,
                std::back_inserter(shaderBuffer_.unicodeIndex));

            copiedSize += (copyEnd - copyStart);
            copyStart = copyEnd;
            
            /* Render part of the text that made the buffer get full. */
            if (shaderBuffer_.transform.size() >= MAX_SHADER_BUFFER_SIZE)
            {
                renderBatchContents();
            }
        }

        /* In order to make use of glScissor we need to render one text buffer item per drawcall instead of
           filling the shader buffer with many text buffer items.. */
        if (shaderBuffer_.transform.size())
        {
            renderBatchContents();
        }
    }
};

void TextRenderer::renderBatchContents()
{
    // Window::setDepthTest(false);

    shader_->setMat4fv("uModelMatv",  shaderBuffer_.transform);
    shader_->setIntv("uCharIdxv",  shaderBuffer_.unicodeIndex);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, shaderBuffer_.transform.size());

    /* Metrics */
    batchCount++;

    clearInternalBuffer();

    Window::setDepthTest(true);
}

void TextRenderer::clearInternalBuffer()
{
    shaderBuffer_.transform.clear();
    shaderBuffer_.unicodeIndex.clear();
}

void TextRenderer::doScissorMask(const utils::TransformPtr tr, const int32_t frameHeight)
{
    glScissor(
        tr->vPos.x,
        frameHeight - tr->vPos.y - tr->vScale.y,
        tr->vScale.x,
        tr->vScale.y);
}
} // namespace msgui::renderer