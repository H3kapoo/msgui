#pragma once

#include <memory>
#include <vector>

#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Transform.hpp"
#include "core/node/FrameState.hpp"
#include "core/Logger.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class WindowFrame;

class AbstractNode;
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
using AbstractNodePVec = std::vector<AbstractNodePtr>;

/* Base class for all UI nodes */
class AbstractNode
{
public:
    enum class NodeType
    {
        COMMON,
        BOX,
        BUTTON,
        SLIDER,
        BOX_DIVIDER,
        BOX_DIVIDER_SEP,
        RECYCLE_LIST,
        SCROLL,
        SCROLL_KNOB
    };

public:
    explicit AbstractNode(Mesh* mesh, Shader* shader, const std::string& name,
        const NodeType nodeType = NodeType::COMMON);
    virtual ~AbstractNode() = default;

    void appendAt(const AbstractNodePtr& node, const uint32_t idx);
    void append(const AbstractNodePtr& node);
    void appendMany(const AbstractNodePVec& nodes);
    void appendMany(std::initializer_list<AbstractNodePtr>& nodes);
    void removeAt(const int32_t idx);
    int32_t removeBy(std::function<bool(AbstractNodePtr)> pred);
    void removeAll();
    AbstractNodePtr remove(const uint32_t& nodeId);
    AbstractNodePVec removeMany(const std::initializer_list<uint32_t>& nodeIds);
    AbstractNodePVec removeMany(const std::vector<uint32_t>& nodeIds);
    AbstractNodePtr remove(const std::string& nodeName);
    AbstractNodePVec remove(const std::initializer_list<std::string>& nodeNames) = delete;
    AbstractNodePVec removeMany(const std::initializer_list<std::string>& nodeNames);
    AbstractNodePVec removeMany(const std::vector<std::string>& nodeNames);
    void printTree(uint32_t currentDepth = 1);

    virtual void setShaderAttributes() = 0;
    // virtual void* getProps() = 0;

    void setShader(Shader* shader);

    Transform& getTransform();
    Shader& getShader();
    Mesh& getMesh();
    std::weak_ptr<AbstractNode> getParent();
    const Transform& getTransform() const;
    const Shader& getShader() const;
    const Mesh& getMesh() const;
    const std::string& getName() const;
    const char* getCName() const;
    uint32_t getId() const;
    NodeType getType() const;
    AbstractNodePVec& getChildren();
    Layout& getLayout();

private: // friend
    friend WindowFrame;

    AbstractNode* getParentRaw();

    virtual void onMouseButtonNotify();
    virtual void onMouseHoverNotify();
    virtual void onMouseDragNotify();
    virtual void onWindowResizeNotify();

private:
    uint32_t genetateNextId() const;
    void resetNodeToDefaults(AbstractNodePtr& node);

protected:
    std::string name_;
    uint32_t id_{0};
    FrameStatePtr state_{nullptr};
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};
    Transform transform_;
    bool isParented_{false};
    NodeType nodeType_{NodeType::COMMON};
    std::weak_ptr<AbstractNode> parent_;
    AbstractNode* parentRaw_{nullptr}; 
    AbstractNodePVec children_;
    Layout layout_;

    Logger log_;
};
} // namespace msgui