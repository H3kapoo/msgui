#include "msgui/layoutEngine/text/SimpleTextLayoutEngine.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui::layoutengine::text
{
void SimpleTextLayoutEngine::process(TextData& data, const bool forceAllDirty)
{
    /* If the current text data isn't dirty (i.e no change in text data or positioning) no need to recalculate.
       However if the calculation is forced we must recalculate all text data. */
    if (!data.isDirty && !forceAllDirty) { return; }

    data.isDirty = false;
    data.pcd.transform.clear();
    data.pcd.unicodeIndex.clear();

    glm::vec3 startPos = data.transformPtr->pos;
    // startPos.x += 10;
    for (char ch : data.text)
    {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, startPos);
        // modelMatrix = glm::scale(modelMatrix, glm::vec3{24, 24, 1});
        modelMatrix = glm::scale(modelMatrix, glm::vec3{16, 16, 1});

        data.pcd.transform.emplace_back(std::move(modelMatrix));
        data.pcd.unicodeIndex.push_back(ch);

        startPos.x += 12;
    }
}
} // namespace msgui::layoutengine::text
