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
class Frame;

/* Base class for all UI nodes */
class AbstractNode
{
public:
    explicit AbstractNode(Mesh* mesh, Shader* shader, const std::string& name);
    virtual ~AbstractNode() = default;

    // Normal
    void append(const std::shared_ptr<AbstractNode>& node);
    void appendMany(const std::vector<std::shared_ptr<AbstractNode>>& nodes);
    void appendMany(std::vector<std::shared_ptr<AbstractNode>>&& nodes);
    void printTree(uint32_t currentDepth = 1);

    // Pure
    virtual void setShaderAttributes() = 0;

    // Getters
    Transform& getTransform();
    Shader& getShader();
    Mesh& getMesh();
    const Transform& getTransform() const;
    const Shader& getShader() const;
    const Mesh& getMesh() const;
    const std::string& getName() const;
    uint32_t getId() const;
    std::vector<std::shared_ptr<AbstractNode>>& getChildren();


private: // friend
    friend Frame;

    // Virtual
    virtual void onMouseButtonNotify();

private:
    // Normal
    uint32_t genetateNextId() const;

protected:
    std::string name_;
    uint32_t id_{0};
    FrameStatePtr state_{nullptr};
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};
    Transform transform_;
    std::weak_ptr<AbstractNode> parent_;
    std::vector<std::shared_ptr<AbstractNode>> children_;

    Logger log_;
};
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
using AbstractNodePVec = std::vector<AbstractNodePtr>;
} // namespace msgui