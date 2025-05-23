#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/Logger.hpp"
#include "msgui/Shader.hpp"
#include "msgui/Mesh.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/events/NodeEventManager.hpp"
#include "msgui/layoutEngine/utils/Transform.hpp"
#include "msgui/Utils.hpp"

namespace msgui
{
using namespace layoutengine;

/*  Only some nodes are privilaged enough to add remove/nodes from the user's perspective.
    Add this in your own class to permit adding/removing nodes from outside.
*/
#define ABSTRACT_NODE_ALLOW_APPEND_REMOVE\
    using AbstractNode::appendAt;\
    using AbstractNode::append;\
    using AbstractNode::appendMany;\
    using AbstractNode::removeAt;\
    using AbstractNode::removeBy;\
    using AbstractNode::removeAll;\
    using AbstractNode::remove;\
    using AbstractNode::removeMany;\
    using AbstractNode::findOneBy;\

class WindowFrame;
class AbstractNode;
using AbstractNodePtr = std::shared_ptr<AbstractNode>;
using AbstractNodePVec = std::vector<AbstractNodePtr>;

/* Base class for all UI nodes. All new nodes can inherit from this for basic functionality. */
class AbstractNode
{
public:
    /* Some elements require different handling depending on type. NodeType provides easy and fast
       access to determine what kind of node this is (instead of dynamic casting) */
    enum class NodeType
    {
        COMMON,
        BOX,
        FLOATING_BOX,
        SLIDER,
        BOX_DIVIDER,
        BOX_DIVIDER_SEP,
        RECYCLE_LIST,
        TREEVIEW,
        DROPDOWN,
        SLIDER_KNOB,
        SCROLL,
        SCROLL_KNOB,
        DROPDOWN_CONTAINTER
    };

public:
    explicit AbstractNode(const std::string& name, const NodeType nodeType = NodeType::COMMON);
    virtual ~AbstractNode() = default;

protected:
    /**
        Appends node at the specified zero based index.

        @param node Node to append
        @param idx Index to append at
     */
    void appendAt(const AbstractNodePtr& node, const int32_t idx);

    /**
        Appends node at the end of container.

        @param node Node to append
     */
    void append(const AbstractNodePtr& node);

    /**
        Appends many nodes at the end of container.

        @param nodes Vector of nodes to append
     */
    void appendMany(const AbstractNodePVec& nodes);

    /**
        Appends many nodes at the end of container.

        @param nodes Initializer list of nodes to append
     */
    void appendMany(std::initializer_list<AbstractNodePtr>& nodes);
    /**
        Removes node at the specified zero based index. Resets node relations.

        @param idx Index to remve at
     */
    void removeAt(const int32_t idx);

    /**
        Removes node that satisfies provided predicate. Resets node relations.

        @param pred Predicate used for checking

        @return Number of nodes that have been removed
     */
    int32_t removeBy(std::function<bool(AbstractNodePtr)> pred);

    /**
        Removes node all nodes. Resets node relations.
     */
    void removeAll();

    /**
        Removes the node with a specific id. Resets node relations.

        @param nodeId Id of node to be removed

        @return Returns the removed node pointer
     */
    AbstractNodePtr remove(const uint32_t& nodeId);

    /**
        Removes the nodes with specific ids. Resets nodes relations.

        @param nodeIds Initializer list of Ids of nodes to be removed

        @return Returns the removed nodes vector
     */
    AbstractNodePVec removeMany(const std::initializer_list<uint32_t>& nodeIds);

    /**
        Removes the nodes with specific ids. Resets nodes relations.

        @param nodeIds Vector of Ids of nodes to be removed

        @return Returns the removed nodes vector
     */
    AbstractNodePVec removeMany(const std::vector<uint32_t>& nodeIds);

    /**
        Removes the node with a specific name. Resets node relations.

        @param nodeName Name of node to be removed

        @return Returns the removed node pointer
     */
    AbstractNodePtr remove(const std::string& nodeName);

    /**
        Removes the nodes with specific names. Resets nodes relations.

        @param nodeNames Initializer list of names of nodes to be removed

        @return Returns the removed nodes vector
     */
    AbstractNodePVec removeMany(const std::initializer_list<std::string>& nodeNames);

    /**
        Removes the nodes with specific names. Resets nodes relations.

        @param nodeNames Vector of names of nodes to be removed

        @return Returns the removed nodes vector
     */
    AbstractNodePVec removeMany(const std::vector<std::string>& nodeNames);

    /**
        Finds first children that satisfies provided predicate.

        @param pred Predicate used for checking

        @return Pointer to found node. Nullptr if not found 
     */
    AbstractNodePtr findOneBy(const std::function<bool(AbstractNodePtr)>& pred);

    /**
        Finds first children that satisfies provided predicate and then casts it to the specified
        template type.

        @param pred Predicate used for checking

        @return Pointer to found node. Nullptr if not found 
     */
    template<typename T>
    requires (std::is_base_of_v<AbstractNode, T>)
    std::shared_ptr<T> findOneBy(const std::function<bool(AbstractNodePtr)>& pred)
    {
        return Utils::as<T>(findOneBy(pred));
    }

public:
    /**
        Each node can have it's own shader attributes and this function allows to set them per node.
    */
    virtual void setShaderAttributes() = 0;

    /**
        Prints a tree view of the current's node children.

        @param currentDepth Specifies at what depth should the printing start from the parent
     */
    void printTree(uint32_t currentDepth = 1);


    /* Setters */
    void setType(const NodeType type);
    void setShader(Shader* shader);
    void setMesh(Mesh* shader);
    void setEventTransparent(const bool isEventTransparent);

    /* Getters */
    utils::Transform& getTransform();
    NodeType getType();
    Shader* getShader();
    Mesh* getMesh();
    FrameStatePtr getState();
    std::weak_ptr<AbstractNode> getParent();
    const utils::Transform& getTransform() const;
    const std::string& getName() const;
    const char* getCName() const;
    uint32_t getId() const;
    NodeType getType() const;
    AbstractNodePVec& getChildren();
    utils::Layout& getLayout();
    events::NodeEventManager& getEvents();
    bool isParented() const;
    bool isEventTransparent() const;

private: // friend
    friend WindowFrame;
    AbstractNode* getParentRaw();

private:
    uint32_t genetateNextId() const;
    void resetNodeToDefaults(AbstractNodePtr& node);
    void resetStatesRecursively(AbstractNodePtr& node);

    /**
        Each node can have it's own behavior when a layout value changes and this function allows to set
        callbacks per node for each layout value. Default behavior is to make layout dirty and request
        another render frame.
    */
    void setupReloadables();

private:
    /* Nullable section and we shall be careful with them */
    Mesh* mesh_{nullptr};
    Shader* shader_{nullptr};
    FrameStatePtr state_{nullptr};
    AbstractNode* parentRaw_{nullptr}; 

protected:
    uint32_t id_{0};
    std::string name_;
    NodeType nodeType_{NodeType::COMMON};
    utils::Transform transform_;
    utils::Layout layout_;
    bool isParented_{false};
    bool isEventTransparent_{false};
    std::weak_ptr<AbstractNode> parent_;
    AbstractNodePVec children_;
    events::NodeEventManager eventManager_;
    Logger log_;
};
} // namespace msgui