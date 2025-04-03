#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "msgui/Logger.hpp"
#include "msgui/Mesh.hpp"
#include "msgui/Shader.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui::renderer::text
{
/* Class responsible for getting text data and translating it into buffers, then rendering the data efficiently. */
class TextRenderer
{
public:
    TextRenderer();

    void render(const glm::mat4& projMat);

private:
    /* Cannot be copied or moved */
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;

    void renderBatchContents();

    Logger log_{"TextRenderer"};
    int32_t fontTexId_{0};
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};
    glm::vec4 color_{1.0f};
    PerCodepointData shaderBuffer_;

    static constexpr int32_t MAX_SHADER_BUFFER_SIZE{100};
};
} // namespace msgui::renderer::text