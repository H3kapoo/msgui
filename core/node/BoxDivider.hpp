#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/BoxDividerSep.hpp"

namespace msgui
{
class BoxDivider : public AbstractNode
{
struct Props;
public:
    BoxDivider(const std::string& name);

    void createSlots(uint32_t slotCount, std::vector<float> initialPercSize);

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    BoxPtr getSlot(uint32_t slotNumber);
    BoxDividerSepPtr getSepatator(uint32_t sepNumber);

private:
    void setShaderAttributes() override;

    void appendBoxContainers(const std::vector<BoxPtr>& boxes);
    void onMouseButtonNotify() override;

    void setupLayoutReloadables();

public:
    Listeners listeners;

private:
    struct Props
    {
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
    };
    Props props;
};
using BoxDividerPtr = std::shared_ptr<BoxDivider>;
} // namespace msgui