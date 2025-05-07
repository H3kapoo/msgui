#include "FloatingBox.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/FrameState.hpp"

namespace msgui
{
FloatingBox::FloatingBox(const std::string& name)
    : AbstractNode(name, NodeType::FLOATING_BOX)
{
    /* Defaults */
    log_ = Logger("FloatingBox(" + name +")");
    setType(AbstractNode::NodeType::FLOATING_BOX);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    color_ = Utils::hexToVec4("#ce7f30ff");
    borderColor_ = Utils::hexToVec4("#ce7f30ff");

    layout_.setNewScale({200_px, 100_px});

    /* Register only the events you need. */
    getEvents().listen<events::LMBRelease, events::InputChannel>(
        std::bind(&FloatingBox::onMouseRelease, this, std::placeholders::_1));
}

void FloatingBox::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void FloatingBox::onMouseRelease(const events::LMBRelease& evt)
{
}

FloatingBox& FloatingBox::setPreferredPosition(const glm::vec2& pos)
{
    preferredPos_ = pos;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;

    return *this;
}

glm::vec2 FloatingBox::getPreferredPos() const { return preferredPos_; }

// BoxWPtr FloatingBox::getContainer() { return box_; }

} // namespace msgui