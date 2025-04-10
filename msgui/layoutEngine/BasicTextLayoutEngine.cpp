#include "BasicTextLayoutEngine.hpp"
#include "msgui/Font.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui::layoutengine
{
void BasicTextLayoutEngine::process(renderer::TextData& data, const bool forceAllDirty)
{
    /* If the calculation is forced we must recalculate all text data. */
    if (shouldSkipData(data) && !forceAllDirty) { return; }

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

bool BasicTextLayoutEngine::shouldSkipData(const renderer::TextData& data) const
{
    /* If the current text data isn't dirty (i.e no change in text data or positioning) no need to recalculate. */
    if (!data.isDirty || !data.transformPtr->vScale.x || !data.transformPtr->vScale.y)
    {
        return true;
    }

    return false;
}

glm::ivec2 BasicTextLayoutEngine::computeTextLengthAndHeight(const renderer::TextData& data) const
{
    const auto fontData = data.fontData;

    glm::ivec2 result{0, 0};
    for (const char ch : data.text)
    {
        const auto& cp = fontData->codePointData[uint32_t(ch)];
        result.x += cp.hAdvance >> 6;
        result.y = std::max(result.y, cp.size.y); // We only need max height for horizontal text.
    }
    return result;
}

} // namespace msgui::layoutengine
