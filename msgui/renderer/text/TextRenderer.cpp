#include "msgui/renderer/text/TextRenderer.hpp"


#include "msgui/FontLoader.hpp"
#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"

#include "msgui/Window.hpp"
#include "msgui/renderer/text/TextBufferStore.hpp"

namespace msgui::renderer::text
{
TextRenderer::TextRenderer()
{
    mesh_ = MeshLoader::loadQuad();
    shader_ = ShaderLoader::loadShader("assets/shader/textInstanced.glsl");
}

void TextRenderer::render(const glm::mat4& projMat)
{
    if (!fontTexId_)
    {
        fontTexId_ = FontLoader().loadFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/UbuntuNerdFont-Regular.ttf", 24);
    }

    Window::setDepthTest(false);
    mesh_->bind();
    shader_->bind();
    shader_->setMat4f("uProjMat", projMat);
    shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, fontTexId_);
    shader_->setVec4f("uColor", color_);
    
    auto& storeBuffer = TextBufferStore::get().buffer();

    // shaderBuffer_.reserve(MAX_SHADER_BUFFER_SIZE);
    for (auto& element : storeBuffer)
    {
        // fill the GPU buffer in order to minimize render calls..
    }

    // just because right now there's only one element
    if (storeBuffer.size())
    {
        auto& front = storeBuffer.front();
        shader_->setMat4fv("uModelMatv", front.pcd.transform);
        shader_->setIntv("uCharIdxv", front.pcd.unicodeIndex);
        
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, front.pcd.unicodeIndex.size());
    }

    Window::setDepthTest(true);

};
} // namespace msgui::renderer::text