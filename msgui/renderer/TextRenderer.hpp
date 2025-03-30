#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "msgui/FontLoader.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Mesh.hpp"
#include "msgui/MeshLoader.hpp"
#include "msgui/Shader.hpp"
#include "msgui/ShaderLoader.hpp"

#include "msgui/Window.hpp"

namespace msgui
{
class TextRenderer
{
struct SoARenderBuffer
{
    std::vector<glm::mat4> transformMatrix;
    std::vector<int32_t> charIdx;
};

public:
    static TextRenderer& get()
    {
        static TextRenderer instance;
        return instance;
    }

    void render(const glm::mat4& projMat)
    {
        Window::setDepthTest(false);
        mesh_->bind();
        shader_->bind();
        shader_->setMat4f("uProjMat", projMat);
        shader_->setMat4fv("uModelMatv", renderBuffer_.transformMatrix);
        shader_->setIntv("uCharIdxv", renderBuffer_.charIdx);
        shader_->setVec4f("uColor", color_);
        shader_->setTexture2DArray("uTextureArray", GL_TEXTURE1, fontTexId_);
        
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderBuffer_.charIdx.size());
        Window::setDepthTest(true);
    };
    
    void pushToBuffer(glm::mat4&& transformMatrix, const int32_t charIdx)
    {
        renderBuffer_.transformMatrix.emplace_back(std::move(transformMatrix));
        renderBuffer_.charIdx.push_back(charIdx);
    };

    void clear()
    {
        renderBuffer_.charIdx.clear();
        renderBuffer_.transformMatrix.clear();
    }
    
private:
    TextRenderer()
    {
        mesh_ = MeshLoader::loadQuad();
        shader_ = ShaderLoader::loadShader("assets/shader/textInstanced.glsl");
        fontTexId_ = FontLoader().loadFont("/home/hekapoo/Documents/probe/newgui/assets/fonts/UbuntuNerdFont-Regular.ttf", 24);
    }

    /* Cannot be copied or moved */
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;

    Logger log_{"TextRenderer"};
    int32_t fontTexId_{0};
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};

    glm::vec4 color_{1.0f};
    SoARenderBuffer renderBuffer_;
};
} // namespace msgui