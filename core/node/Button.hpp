#pragma once

#include <functional>
#include <memory>

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/Texture.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    // Overrides 
    void setShaderAttributes() override;

    // Virtual Getters
    void* getProps() override;

    // Getters
    Listeners& getListeners();

private:
    // Override Notifiers
    void onMouseButtonNotify() override;

private:
    TexturePtr btnTex;
    Listeners listeners_;
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui