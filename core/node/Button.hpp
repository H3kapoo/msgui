#pragma once

#include <memory>

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/Texture.hpp"
#include "core/Utils.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{1.0f};
        AR<std::string> texture;
    };

public:
    Button(const std::string& name);
    void setShaderAttributes() override;
    void* getProps() override;

private:
    void onMouseButtonNotify() override;

    void setupReloadables();

    // Buttons shall not have user added children
    void append();
    void appendMany();
    void remove();
    void removeMany();

public:
    Listeners listeners;
    Props props;

private:
    TexturePtr btnTex_;
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui