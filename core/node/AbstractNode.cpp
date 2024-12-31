#include "AbstractNode.hpp"

namespace msgui
{
AbstractNode::AbstractNode(Mesh* mesh, Shader* shader, const std::string& name, const NodeType nodeType)
        : name_(name)
        , id_(genetateNextId())
        , mesh_(mesh)
        , shader_(shader)
        , nodeType_(nodeType)
{}

// ---- Normal ---- //
void AbstractNode::append(const std::shared_ptr<AbstractNode>& node)
{   
    if (!node)
    {
        log_.errorLn("Trying to nullptr node!");
        return;
    }

    if (node->id_ == id_ || node->isParented_)
    {
        log_.warnLn("Trying to append already parented nodeId: %d", node->id_);
        return;
    }

    // isParented_ is used as a quick mean to check even before all the layout "stabilization"
    // if this node has been parented already so that we can avoid "double parenting" this node
    // to yet another node.
    // Only when the layout will be calculated again (next frame) will the parentNode & state be
    // populated accordingly if absent.
    node->isParented_ = true;

    children_.push_back(node);
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

AbstractNode::NodeType AbstractNode::getType() const
{
    return nodeType_;
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