#include "AbstractNode.hpp"

namespace msgui
{
AbstractNode::AbstractNode(Mesh* mesh, Shader* shader, const std::string& name)
        : name_(name)
        , id_(genetateNextId())
        , mesh_(mesh)
        , shader_(shader)
{}

// ---- Normal ---- //
void AbstractNode::append(const std::shared_ptr<AbstractNode>& node)
{
    if (node->id_ == id_ || node->parent_.lock())
    {
        return;
    }

    children_.push_back(node);

    // Only if this node is parented (by parent hopping) to the Frame
    if (state_)
    {
        state_->isLayoutDirty = true;
    }
}

void AbstractNode::appendMany(const std::vector<std::shared_ptr<AbstractNode>>& nodes)
{
    for (const auto& node : nodes)
    {
        append(node);
    }
}

void AbstractNode::appendMany(std::vector<std::shared_ptr<AbstractNode>>&& nodes)
{
    for (const auto& node : nodes)
    {
        append(node);
    }
}

void AbstractNode::printTree(uint32_t currentDepth)
{
    currentDepth ? log_.raw("") : log_.infoLn("");
    for (uint32_t i = 0; i < currentDepth; i++)
    {
        log_.raw("    ");
    }

    log_.raw("\\---");
    log_.raw("%s (l:%d) (r:%d)\n", name_.c_str(), (int32_t)transform_.pos.z, currentDepth);
    // details();

    for (const auto& node : children_)
    {
        node->printTree(currentDepth + 1);
    }
}

// ---- Getters ---- //
Transform& AbstractNode::getTransform()
{
    return transform_;
}

Shader& AbstractNode::getShader()
{
    return *shader_;
}

Mesh& AbstractNode::getMesh()
{
    return *mesh_;
}

const Transform& AbstractNode::getTransform() const
{
    return transform_;
}

const Shader& AbstractNode::getShader() const
{
    return *shader_;
}

const Mesh& AbstractNode::getMesh() const
{
    return *mesh_;
}

const std::string& AbstractNode::getName() const
{
    return name_;
}

uint32_t AbstractNode::getId() const
{
    return id_;
}

std::vector<std::shared_ptr<AbstractNode>>& AbstractNode::getChildren()
{
    return children_;
}

// ---- Virtual Private ---- //
void AbstractNode::onMouseButtonNotify() {}

// ---- Normal Private ---- //
uint32_t AbstractNode::genetateNextId() const
{
    static uint32_t id = 0;
    return ++id;
}
} // namespace msgui