#include "AbstractNode.hpp"

#include <algorithm>

#include "core/node/FrameState.hpp"
#include "core/Utils.hpp"

namespace msgui
{
AbstractNode::AbstractNode(const std::string& name, const NodeType nodeType)
        : id_(genetateNextId())
        , name_(name)
        , nodeType_(nodeType)
{}

void AbstractNode::appendAt(const std::shared_ptr<AbstractNode>& node, const int32_t idx)
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

    /* isParented_ is used as a quick mean to check even before all the layout "stabilization" if this node has been
       parented already so that we can avoid "double parenting" to yet another node.
       Only when the layout will be calculated again (next frame) will the parentNode & state be
       populated accordingly if absent. */
    node->isParented_ = true;

    children_.insert(children_.begin() + idx, node);
    if (state_)
    {
        state_->isLayoutDirty = true;
        state_->layoutStoreNeedsRecreate = true;
    }
}

void AbstractNode::append(const std::shared_ptr<AbstractNode>& node)
{
    appendAt(node, children_.size());
}

void AbstractNode::appendMany(const std::vector<std::shared_ptr<AbstractNode>>& nodes)
{
    for (const auto& node : nodes)
    {
        append(node);
    }
}

void AbstractNode::appendMany(std::initializer_list<std::shared_ptr<AbstractNode>>& nodes)
{
    for (const auto& node : nodes)
    {
        append(node);
    }
}

void AbstractNode::removeAt(const int32_t idx)
{
    if (idx < 0 || idx > (int32_t)children_.size() - 1) { return; }

    resetNodeToDefaults(children_[idx]);
    children_.erase(children_.begin() + idx);
}

int32_t AbstractNode::removeBy(std::function<bool(AbstractNodePtr)> pred)
{
    return std::erase_if(children_, [this, &pred](AbstractNodePtr node)
    {
        if (!pred(node)) { return false; }

        resetNodeToDefaults(node);

        return true;
    });
}

void AbstractNode::removeAll()
{
    removeBy([](auto){ return true; });
}

std::shared_ptr<AbstractNode> AbstractNode::remove(const uint32_t& nodeId)
{
    const auto it = std::find_if(children_.begin(), children_.end(),
        [&nodeId](const std::shared_ptr<AbstractNode>& node)
        {
            return node->getId() == nodeId;
        });

    if (it == children_.end())
    {
        return nullptr;
    }

    resetNodeToDefaults(*it);

    /* Transfer ownership out of the vector and erase remaining iterator */
    std::shared_ptr<AbstractNode> returned = std::move(*it);
    children_.erase(it);

    return returned;
}

std::vector<std::shared_ptr<AbstractNode>> AbstractNode::removeMany(const std::initializer_list<uint32_t>& nodeIds)
{
    std::vector<std::shared_ptr<AbstractNode>> removedNodes;
    for (const auto& nodeId : nodeIds)
    {
        if (const auto it = remove(nodeId))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

std::vector<std::shared_ptr<AbstractNode>> AbstractNode::removeMany(const std::vector<uint32_t>& nodeIds)
{
    std::vector<std::shared_ptr<AbstractNode>> removedNodes;
    for (const auto& nodeId : nodeIds)
    {
        if (const auto it = remove(nodeId))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

std::shared_ptr<AbstractNode> AbstractNode::remove(const std::string& nodeName)
{
    const auto it = std::find_if(children_.begin(), children_.end(),
        [&nodeName](const std::shared_ptr<AbstractNode>& node)
        {
            return node->getName() == nodeName;
        });

    if (it == children_.end())
    {
        return nullptr;
    }

    resetNodeToDefaults(*it);

    /* Transfer ownership out of the vector and erase remaining iterator */
    std::shared_ptr<AbstractNode> returned = std::move(*it);
    children_.erase(it);

    return returned;
}

std::vector<std::shared_ptr<AbstractNode>> AbstractNode::removeMany(
    const std::initializer_list<std::string>& nodeNames)
{
    std::vector<std::shared_ptr<AbstractNode>> removedNodes;
    for (const auto& nodeName : nodeNames)
    {
        if (const auto it = remove(nodeName))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

std::vector<std::shared_ptr<AbstractNode>> AbstractNode::removeMany(
    const std::vector<std::string>& nodeNames)
{
    std::vector<std::shared_ptr<AbstractNode>> removedNodes;
    for (const auto& nodeName : nodeNames)
    {
        if (const auto it = remove(nodeName))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

void AbstractNode::printTree(uint32_t currentDepth)
{
    currentDepth ? log_.raw("") : log_.infoLn("");
    for (uint32_t i = 0; i < currentDepth - 1; i++)
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

void AbstractNode::setShader(Shader* shader)
{
    shader_ = shader;
}

void AbstractNode::setMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

Transform& AbstractNode::getTransform()
{
    return transform_;
}

Shader* AbstractNode::getShader()
{
    if (!shader_) { Utils::fatalExit(std::string{__PRETTY_FUNCTION__} + std::string{"::"} + name_ ); }
    return shader_;
}

Mesh* AbstractNode::getMesh()
{
    if (!mesh_) { Utils::fatalExit(std::string{__PRETTY_FUNCTION__} + std::string{"::"} + name_ ); }
    return mesh_;
}

FrameStatePtr AbstractNode::getState()
{
    return state_;
}

std::weak_ptr<AbstractNode> AbstractNode::getParent()
{
    return parent_;
}

AbstractNode* AbstractNode::getParentRaw()
{
    /* To be used ONLY by critical paths */
    return parentRaw_;
}

const Transform& AbstractNode::getTransform() const
{
    return transform_;
}

const std::string& AbstractNode::getName() const
{
    return name_;
}

const char* AbstractNode::getCName() const
{
    return name_.c_str();
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

Layout& AbstractNode::getLayout()
{
    return layout_;
}

void AbstractNode::onMouseButtonNotify() {}

void AbstractNode::onMouseHoverNotify() {}

void AbstractNode::onMouseDragNotify() {}

void AbstractNode::onWindowResizeNotify() {}

uint32_t AbstractNode::genetateNextId() const
{
    static uint32_t id = 0;
    return ++id;
}

void AbstractNode::resetNodeToDefaults(std::shared_ptr<AbstractNode>& node)
{
    /* Notify layout */
    if (node->state_)
    {
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    }

    /* Reset to defaults */
    node->state_ = nullptr;
    node->isParented_ = false;
    node->parent_.reset();
    node->parentRaw_ = nullptr;

}
} // namespace msgui