#include "FloatingBox.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"

namespace msgui
{
FloatingBox::FloatingBox(const std::string& name)
    : AbstractNode(name, NodeType::FLOATING_BOX)
{
    log_ = Logger("FloatingBox(" + name +")");
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    box_ = Utils::make<Box>("InternalBox");
    box_->getLayout().setScaleType(utils::Layout::ScaleType::REL)
        .setScale({1.0f, 1.0f})
        .setAlignChild(utils::Layout::Align::CENTER);
    append(box_);

    /* Defaults */
    color_ = Utils::hexToVec4("#ce7f30");
    borderColor_ = Utils::hexToVec4("#ce7f30");

    layout_.setScale({200, 100});

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
    // setPreferredPosition({getState()->mouseX, getState()->mouseY});
}

FloatingBox& FloatingBox::setPreferredPosition(const glm::vec2& pos)
{
    preferredPos_ = pos;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;

    return *this;
}

glm::vec2 FloatingBox::getPreferredPos() const { return preferredPos_; }

BoxWPtr FloatingBox::getContainer() { return box_; }

} // namespace msgui