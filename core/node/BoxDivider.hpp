#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/BoxDividerSep.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class BoxDivider : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
    };

public:
    BoxDivider(const std::string& name);

    // void appendBoxContainer(const BoxPtr& box);
    // void append(const BoxPtr& box);
    void createSlots(uint32_t slotCount, std::vector<float> initialPercSize);

    void setShaderAttributes() override;
    void* getProps() override;
    BoxPtr getSlot(uint32_t slotNumber);
    BoxDividerSepPtr getSepatator(uint32_t sepNumber);

private:
    void appendBoxContainers(const std::vector<BoxPtr>& boxes);
    void onMouseButtonNotify() override;

    void setupReloadables();

    // BoxDividers shall not have user added children
    // void append(const AbstractNodePtr&) override {};
    // void appendMany(const AbstractNodePVec&) override {};
    // void appendMany(std::initializer_list<AbstractNodePtr>& ) override {};
    // AbstractNodePtr remove(const uint32_t&) override { return nullptr; };
    // AbstractNodePVec removeMany(const std::initializer_list<uint32_t>&) override { return {}; };
    // AbstractNodePVec removeMany(const std::vector<uint32_t>&) override { return {}; };
    // AbstractNodePtr remove(const std::string&) override { return nullptr; };
    // AbstractNodePVec removeMany(const std::initializer_list<std::string>&) override { return {}; };

public:
    Listeners listeners;
    Props props;

private:

};
using BoxDividerPtr = std::shared_ptr<BoxDivider>;
} // namespace msgui