#pragma once

#include <functional>
#include <memory>

#include "AbstractNode.hpp"
#include "core/Texture.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    // Overrides 
    void setShaderAttributes() override;

    // Listeners
    void setMouseClickListener(std::function<void()> cb);

private:
    // Overrides
    void onMouseButtonNotify() override;

private:
    TexturePtr btnTex;
    std::function<void()> mouseClickCb_{nullptr};
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui