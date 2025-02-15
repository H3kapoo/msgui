#pragma once

#include "AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/BoxDividerSep.hpp"

namespace msgui
{
class BoxDivider : public AbstractNode
{
public:
    BoxDivider(const std::string& name);

    /**
        Creates *slotCount* slots (panes) of initialPercSize specified size each (depending on index).

        @param slotCount Number of slots (panes) to be created
        @param initialPercSize Array specifying initial percentage size of each pane (0<x<1)
    */
    void createSlots(uint32_t slotCount, std::vector<float> initialPercSize);

    BoxDivider& setColor(const glm::vec4& color);
    BoxDivider& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    BoxWPtr getSlot(uint32_t slotNumber);
    BoxDividerSepWPtr getSepatator(uint32_t sepNumber);

private:
    BoxDivider(const BoxDivider&) = delete;
    BoxDivider(BoxDivider&&) = delete;
    BoxDivider& operator=(const BoxDivider&) = delete;
    BoxDivider& operator=(BoxDivider&&) = delete;

    void setShaderAttributes() override;

    void appendBoxContainers(const std::vector<BoxPtr>& boxes);
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
};
using BoxDividerPtr = std::shared_ptr<BoxDivider>;
} // namespace msgui