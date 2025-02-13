#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/BoxDividerSep.hpp"

namespace msgui
{
class BoxDivider : public AbstractNode
{
public:
    BoxDivider(const std::string& name);

    void createSlots(uint32_t slotCount, std::vector<float> initialPercSize);

    BoxDivider& setColor(const glm::vec4& color);
    BoxDivider& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    BoxPtr getSlot(uint32_t slotNumber);
    BoxDividerSepPtr getSepatator(uint32_t sepNumber);
    Listeners& getListeners();

private:
    void setShaderAttributes() override;

    void appendBoxContainers(const std::vector<BoxPtr>& boxes);

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    Listeners listeners_;
};
using BoxDividerPtr = std::shared_ptr<BoxDivider>;
} // namespace msgui