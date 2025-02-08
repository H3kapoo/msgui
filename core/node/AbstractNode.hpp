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
        BUTTON,
        SLIDER,
        BOX_DIVIDER,
        BOX_DIVIDER_SEP,
        RECYCLE_LIST,
        DROPDOWN,
        SCROLL,
        SCROLL_KNOB
    };

public:
    explicit AbstractNode(const std::string& name, const NodeType nodeType = NodeType::COMMON);
    virtual ~AbstractNode() = default;

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
    AbstractNodePtr findOneBy(std::function<bool(AbstractNodePtr)> pred);

    /**
        Prints a tree view of the current's node children.

        @param currentDepth Specifies at what depth should the printing start from the parent
     */
    void printTree(uint32_t currentDepth = 1);

    virtual void setShaderAttributes() = 0;

    /* Setters */
    void setShader(Shader* shader);
    void setMesh(Mesh* shader);

    /* Getters */
    Transform& getTransform();
    Shader* getShader();
    Mesh* getMesh();
    FrameStatePtr getState();
    std::weak_ptr<AbstractNode> getParent();
    const Transform& getTransform() const;
    const std::string& getName() const;
    const char* getCName() const;
    uint32_t getId() const;
    NodeType getType() const;
    AbstractNodePVec& getChildren();
    Layout& getLayout();

private: // friend
    friend WindowFrame;

    AbstractNode* getParentRaw();

    // /* Special for notifying nodes (RecycleList mostly) of layout update */
    // virtual void onLayoutUpdateNotify();

    /* Event consumers */
    virtual void onMouseButtonNotify();
    virtual void onMouseHoverNotify();
    virtual void onMouseDragNotify();
    virtual void onWindowResizeNotify();

private:
    uint32_t genetateNextId() const;
    void resetNodeToDefaults(AbstractNodePtr& node);

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
    Transform transform_;
    Layout layout_;
    bool isParented_{false};
    std::weak_ptr<AbstractNode> parent_;
    AbstractNodePVec children_;
    Logger log_;
};
} // namespace msgui