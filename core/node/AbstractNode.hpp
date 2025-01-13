#pragma once

#include <memory>
#include <vector>

#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Transform.hpp"
#include "core/node/FrameState.hpp"
#include "core/Logger.hpp"

namespace msgui
{
// Friend
class WindowFrame;

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
        SCROLL,
        SCROLL_KNOB
    };

public:
    explicit AbstractNode(Mesh* mesh, Shader* shader, const std::string& name,
        const NodeType nodeType = NodeType::COMMON);
    virtual ~AbstractNode() = default;

    void append(const std::shared_ptr<AbstractNode>& node);
    void appendMany(const std::vector<std::shared_ptr<AbstractNode>>& nodes);
    void appendMany(std::initializer_list<std::shared_ptr<AbstractNode>>& nodes);
    std::shared_ptr<AbstractNode>              remove(const uint32_t& nodeId);
    std::vector<std::shared_ptr<AbstractNode>> removeMany(const std::initializer_list<uint32_t>& nodeIds);
    std::vector<std::shared_ptr<AbstractNode>> removeMany(const std::vector<uint32_t>& nodeIds);
    std::shared_ptr<AbstractNode>              remove(const std::string& nodeName);
    std::vector<std::shared_ptr<AbstractNode>> remove(const std::initializer_list<std::string>& nodeNames) = delete;
    std::vector<std::shared_ptr<AbstractNode>> removeMany(const std::initializer_list<std::string>& nodeNames);
    std::vector<std::shared_ptr<AbstractNode>> removeMany(const std::vector<std::string>& nodeNames);
    void printTree(uint32_t currentDepth = 1);

    virtual void setShaderAttributes() = 0;
    virtual void* getProps() = 0;

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
    std::vector<std::shared_ptr<AbstractNode>>& getChildren();

private: // friend
    friend WindowFrame;

    AbstractNode* getParentRaw();

    virtual void onMouseButtonNotify();
    virtual void onMouseHoverNotify();
    virtual void onMouseDragNotify();

private:
    uint32_t genetateNextId() const;

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
    std::vector<std::shared_ptr<AbstractNode>> children_;

    Logger log_;
};
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
using AbstractNodePVec = std::vector<AbstractNodePtr>;
} // namespace msgui