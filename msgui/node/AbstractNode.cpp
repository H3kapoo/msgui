#include "AbstractNode.hpp"

#include <algorithm>

#include "msgui/node/FrameState.hpp"
#include "msgui/Utils.hpp"

namespace msgui
{
AbstractNode::AbstractNode(const std::string& name, const NodeType nodeType)
        : id_(genetateNextId())
        , name_(name)
        , nodeType_(nodeType)
{
    setupReloadables();
}

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
        state_->layoutPassActions |= ELayoutPass::EVERYTHING_NODE;
    }
}

void AbstractNode::append(const std::shared_ptr<AbstractNode>& node)
{
    appendAt(node, children_.size());
}

void AbstractNode::appendMany(const AbstractNodePVec& nodes)
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
    removeBy([](const AbstractNodePtr& node ){ return node->getType() != AbstractNode::NodeType::SCROLL; });
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

AbstractNodePVec AbstractNode::removeMany(const std::initializer_list<uint32_t>& nodeIds)
{
    AbstractNodePVec removedNodes;
    for (const auto& nodeId : nodeIds)
    {
        if (const auto it = remove(nodeId))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

AbstractNodePVec AbstractNode::removeMany(const std::vector<uint32_t>& nodeIds)
{
    AbstractNodePVec removedNodes;
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

AbstractNodePVec AbstractNode::removeMany(
    const std::initializer_list<std::string>& nodeNames)
{
    AbstractNodePVec removedNodes;
    for (const auto& nodeName : nodeNames)
    {
        if (const auto it = remove(nodeName))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

AbstractNodePVec AbstractNode::removeMany(
    const std::vector<std::string>& nodeNames)
{
    AbstractNodePVec removedNodes;
    for (const auto& nodeName : nodeNames)
    {
        if (const auto it = remove(nodeName))
        {
            removedNodes.emplace_back(it);
        }
    }

    return removedNodes;
}

AbstractNodePtr AbstractNode::findOneBy(const std::function<bool(AbstractNodePtr)>& pred)
{
    const auto it = std::find_if(children_.begin(), children_.end(), pred);
    if (it == children_.end()) { return nullptr; }
    return *it;
}

void AbstractNode::printTree(uint32_t currentDepth)
{
    /*
        CD - Calculated Depth according to the Z transform component (used in rendering)
        RD - Relative Depth is the depth of the node relative to the printed tree node

        Note: CD will be valid only after layout calculations ran at least once after each tree structure change.
    */
    currentDepth ? log_.raw("") : log_.infoLn("");
    for (uint32_t i = 0; i < currentDepth - 1; i++)
    {
        log_.raw("    ");
    }

    log_.raw("\\---");
    log_.raw("'%s' (ID: %d) (CD: %d) (RD: %d)\n", name_.c_str(), getId(), (int32_t)transform_.pos.z, currentDepth);

    for (const auto& node : children_)
    {
        node->printTree(currentDepth + 1);
    }
}

void AbstractNode::setType(const AbstractNode::NodeType type)
{
    nodeType_ = type;
}

void AbstractNode::setShader(Shader* shader)
{
    shader_ = shader;
}

void AbstractNode::setMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

void AbstractNode::setEventTransparent(const bool isEventTransparent)
{
    isEventTransparent_ = isEventTransparent;
}

utils::Transform& AbstractNode::getTransform()
{
    return transform_;
}

AbstractNode::NodeType AbstractNode::getType()
{
    return nodeType_;
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

const utils::Transform& AbstractNode::getTransform() const
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

AbstractNodePVec& AbstractNode::getChildren()
{
    return children_;
}

utils::Layout& AbstractNode::getLayout()
{
    return layout_;
}

events::NodeEventManager& AbstractNode::getEvents() { return eventManager_; }

bool AbstractNode::isParented() const { return isParented_; }

bool AbstractNode::isEventTransparent() const { return isEventTransparent_; };

uint32_t AbstractNode::genetateNextId() const
{
    static uint32_t id = 0;
    return ++id;
}

void AbstractNode::resetNodeToDefaults(AbstractNodePtr& node)
{
    // TODO: Very important => reset the state_ of the childen nodes recursively
    // as they are no longer part of the frame. Not resetting the state will mean
    // that on re-addition (potentially on another parent) might cause invalid parents
    // or invalid depths.

    /* Notify layout */
    if (node->state_)
    {
        MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    }

    /* Reset to defaults */
    node->isParented_ = false;
    node->parent_.reset();
    node->parentRaw_ = nullptr;

    resetStatesRecursively(node);
}

void AbstractNode::resetStatesRecursively(AbstractNodePtr& node)
{
    /* Some parameters like state and vScale need to be reset all the way down since:
        1. There's no more state to rely on, you're detached from the main UI tree
        2. You can no longer be visible so your vScale needs to be reset
    */
    node->state_ = nullptr;
    node->transform_.vScale = {0, 0};
    for (auto& ch : node->getChildren())
    {
        resetStatesRecursively(ch);
    }
}

void AbstractNode::setupReloadables()
{
    auto updateCb = [this](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridPosRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
}
} // namespace msgui