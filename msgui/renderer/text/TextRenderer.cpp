#include "msgui/renderer/text/TextRenderer.hpp"


#include "msgui/FontLoader.hpp"
#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"

#include "msgui/Window.hpp"
#include "msgui/renderer/text/TextBufferStore.hpp"
#include <iterator>

namespace msgui::renderer::text
{
TextRenderer::TextRenderer()
{
    mesh_ = MeshLoader::loadQuad();
    shader_ = ShaderLoader::loadShader("assets/shader/textInstanced.glsl");

    shaderBuffer_.transform.reserve(MAX_SHADER_BUFFER_SIZE);
    shaderBuffer_.unicodeIndex.reserve(MAX_SHADER_BUFFER_SIZE);
}

void TextRenderer::render(const glm::mat4& projMat)
{
    // TODO: Needs to be moved from here
    if (!fontTexId_)
    {
        fontTexId_ = FontLoader().loadFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/UbuntuNerdFont-Regular.ttf", 16);
    }
    static int32_t batchCnt = 0;

    mesh_->bind();
    shader_->bind();
    shader_->setMat4f("uProjMat", projMat);
    shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, fontTexId_);
    shader_->setVec4f("uColor", color_);
    
    auto& storeBuffer = TextBufferStore::get().buffer();
    for (auto& element : storeBuffer)
    {
        /* Copy all if the element data can fit inside the buffer without exceeding the limit. */
        if (shaderBuffer_.transform.size() + element.pcd.transform.size() <= MAX_SHADER_BUFFER_SIZE)
        {
            std::copy(element.pcd.transform.begin(), element.pcd.transform.end(),
                 std::back_inserter(shaderBuffer_.transform));
            std::copy(element.pcd.unicodeIndex.begin(), element.pcd.unicodeIndex.end(),
                std::back_inserter(shaderBuffer_.unicodeIndex));
        }
        /* Otherwise it needs to be split into pieces. */
        else
        {
            int32_t copiedSize = 0;
            while ((int32_t)element.pcd.transform.size() - 1 > copiedSize)
            {
                int32_t spaceLeft = MAX_SHADER_BUFFER_SIZE - shaderBuffer_.transform.size();
                int32_t copyEnd = copiedSize + spaceLeft;
                std::copy(element.pcd.transform.begin() + copiedSize, element.pcd.transform.begin() + copyEnd,
                     std::back_inserter(shaderBuffer_.transform));
                std::copy(element.pcd.unicodeIndex.begin() + copiedSize, element.pcd.unicodeIndex.begin() + copyEnd,
                     std::back_inserter(shaderBuffer_.unicodeIndex));
                copiedSize += spaceLeft;

                if (spaceLeft <= 0)
                {
                    renderBatchContents();
                    ++batchCnt;
                }
            }
        }

        /* In case the element fit perfectly inside the buffer and there's no more space in it: render immediatelly. */
        if (shaderBuffer_.transform.size() >= MAX_SHADER_BUFFER_SIZE)
        {
            renderBatchContents();
            ++batchCnt;
        }
    }
    
    /* Render the elements that didnt reach max batch capacity. */
    if (shaderBuffer_.transform.size())
    {
        renderBatchContents();
    }
    log_.debugLn("Rendered batch partial %d", batchCnt);

    Window::setDepthTest(true);
    batchCnt = 0;
};

void TextRenderer::renderBatchContents()
{
    Window::setDepthTest(false);

    shader_->setMat4fv("uModelMatv",  shaderBuffer_.transform);
    shader_->setIntv("uCharIdxv",  shaderBuffer_.unicodeIndex);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, shaderBuffer_.transform.size());

    shaderBuffer_.transform.clear();
    shaderBuffer_.unicodeIndex.clear();

    Window::setDepthTest(true);
}
} // namespace msgui::renderer::text