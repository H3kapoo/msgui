#include "msgui/renderer/text/TextRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
    // fontTexId_ = FontLoader().loadFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/UbuntuNerdFont-Regular.ttf", 24);
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
        // calculate text positions & char index
        glm::vec3 startPos = element.pos;
        for (char ch : element.text)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, startPos);
            modelMatrix = glm::scale(modelMatrix, glm::vec3{24, 24, 1});

            element.pcd.transform.emplace_back(std::move(modelMatrix));
            element.pcd.unicodeIndex.push_back(ch);

            startPos.x += 50;
        }
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