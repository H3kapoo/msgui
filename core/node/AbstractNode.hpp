#pragma once

#include <memory>
#include <vector>

#include "core/Mesh.hpp"
#include "core//Shader.hpp"
#include "core/Transform.hpp"
#include "core/node/FrameState.hpp"
#include "core/Logger.hpp"

namespace msgui
{
class Frame;
class AbstractNode : public std::enable_shared_from_this<AbstractNode>
{
public:
    explicit AbstractNode(Mesh* mesh, Shader* shader, const std::string& name);
    virtual ~AbstractNode() = default;

    void append(const std::shared_ptr<AbstractNode>& node);
    void appendMany(const std::vector<std::shared_ptr<AbstractNode>>& nodes);
    void appendMany(std::vector<std::shared_ptr<AbstractNode>>&& nodes);

    virtual void setShaderAttributes(){};

    Transform& getTransform();
    Shader& getShader();
    Mesh& getMesh();
    const Transform& getTransform() const;
    const Shader& getShader() const;
    const Mesh& getMesh() const;
    uint32_t getDepth() const;
    std::vector<std::shared_ptr<AbstractNode>>& getChildren();

    void printTree(uint32_t currentDepth = 0);

private: // friend
    friend Frame;
    virtual void onMouseButtonNotify();

private:
    uint32_t genetateNextId() const;

protected:
    std::string name_;
    uint32_t id_{0};
    uint32_t depth_{0};
    FrameStatePtr state_{nullptr};
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};
    Transform transform_;
    std::weak_ptr<AbstractNode> parent_;
    std::vector<std::shared_ptr<AbstractNode>> children_;

    Logger log_;
};
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
} // namespace msgui