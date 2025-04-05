#include "BasicTextLayoutEngine.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui::layoutengine
{
void BasicTextLayoutEngine::process(renderer::TextData& data, const bool forceAllDirty)
{
    /* If the current text data isn't dirty (i.e no change in text data or positioning) no need to recalculate.
       However if the calculation is forced we must recalculate all text data. */
    if (!data.isDirty && !forceAllDirty) { return; }

    data.isDirty = false;
    data.pcd.transform.clear();
    data.pcd.unicodeIndex.clear();

    auto fontData = data.fontData;
    glm::vec3 startPos = data.transformPtr->pos;
    float inc = 0.001f;
    for (char ch : data.text)
    {
        inc += 0.001f;
        int32_t idx = ch;
        float x = startPos.x + fontData->codePointData[idx].bearing.x;
        float y = startPos.y - fontData->codePointData[idx].bearing.y + fontData->fontSize;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3{x, y, startPos.z + inc});
        modelMatrix = glm::scale(modelMatrix, glm::vec3{fontData->fontSize, fontData->fontSize, 1});

        data.pcd.transform.emplace_back(std::move(modelMatrix));
        data.pcd.unicodeIndex.push_back(ch);

        startPos.x += fontData->codePointData[idx].hAdvance >> 6;
    }
}
} // namespace msgui::layoutengine
