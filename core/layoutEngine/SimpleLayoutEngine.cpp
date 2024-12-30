#include "SimpleLayoutEngine.hpp"

namespace msgui
{
// ---- Normal ---- //
void SimpleLayoutEngine::process(const AbstractNodePtr& parent)
{
    AbstractNodePVec& children = parent->getChildren();

    // log_.infoLn("On it boss %s %d %ld", parent->getName().c_str(), parent->getId(), children.size());
    auto& pPos = parent->getTransform().pos;
    auto& pScale = parent->getTransform().scale;

    int32_t startX = pPos.x;
    int32_t startY = pPos.y;

    int32_t rollingX = startX;
    for (auto& ch : children)
    {
        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;
        rollingX += scale.x;
        if (rollingX > pScale.x)
        {
            startX = 0;
            startY += scale.y;
            rollingX = pPos.x;
        }

        pos.x = startX;
        pos.y = startY;
        startX += scale.x;
    }
}
} // msgui