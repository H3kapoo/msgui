#include "BasicTextLayoutEngine.hpp"
#include "msgui/Font.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui::layoutengine
{
void BasicTextLayoutEngine::process(renderer::TextData& data, const bool forceAllDirty)
{
    /* No point in computing anything if the parent ain't event visible. */
    if (data.transformPtr->vScale.x <= 0 || data.transformPtr->vScale.y <= 0) { return; }
    
    /* If the calculation is forced we must recalculate text data even if it's not dirty (from user pov). */
    if (!data.isDirty && !forceAllDirty) { return; }

    data.isDirty = false;
    data.pcd.transform.clear();
    data.pcd.unicodeIndex.clear();

    float incZ = 0.001f;
    glm::ivec3 startPos = data.transformPtr->pos;
    data.textBounds = computeTextLengthAndHeight(data);
    FontPtr& fontData = data.fontData;

    startPos.x += data.transformPtr->scale.x * 0.5f - data.textBounds.x * 0.5f;
    startPos.y += data.transformPtr->scale.y * 0.5f - data.textBounds.y * 0.5f;
    int32_t lineNo = 1;
    for (char ch : data.text)
    {
        incZ += 0.001f;
        const auto& cp = fontData->codePointData[uint32_t(ch)];
        // if (startPos.x + cp.bearing.x + cp.size.x > data.transformPtr->pos.x + data.transformPtr->scale.x)
        // {
        //     startPos.x = data.transformPtr->pos.x;
        //     lineNo++;
        // }
        const float x = startPos.x + cp.bearing.x;
        const float y = startPos.y - cp.bearing.y + fontData->fontSize * lineNo;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3{x, y, startPos.z + incZ});
        modelMatrix = glm::scale(modelMatrix, glm::vec3{fontData->fontSize, fontData->fontSize, 1});

        int32_t advance = cp.hAdvance >> 6;
        startPos.x += advance;

        data.pcd.transform.emplace_back(std::move(modelMatrix));
        data.pcd.unicodeIndex.push_back(ch);
    }
}

glm::ivec2 BasicTextLayoutEngine::computeTextLengthAndHeight(const renderer::TextData& data) const
{
    const auto fontData = data.fontData;

    glm::ivec2 result{0, 0};
    for (const char ch : data.text)
    {
        const auto& cp = fontData->codePointData[uint32_t(ch)];
        result.x += cp.hAdvance >> 6;
        result.y = std::max(result.y, cp.size.y - 1); // We only need max height for horizontal text.
    }
    return result;
}
} // namespace msgui::layoutengine
