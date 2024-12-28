#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Logger.hpp"

namespace msgui
{
using TextureUnitId = uint32_t;
using TextureTargetType = uint32_t;

class Shader
{
public:
    explicit Shader(const uint32_t shaderId, const std::string& shaderName);
    ~Shader();
    Shader& operator=(Shader&& other);

    void setTexture1D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;
    void setTexture2D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;
    void setTexture3D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;
    void setTexture1DArray(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;
    void setTexture2DArray(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;
    void setInt(const std::string& name, const int32_t value) const;
    void setVec3f(const std::string& name, const glm::vec3& value) const;
    void setVec4f(const std::string& name, const glm::vec4& value) const;
    void setMat4f(const std::string& name, const glm::mat4& value) const;
    void bind() const;
    void unbind() const;

    uint32_t getShaderId() const;

private:
    Shader(Shader&& other) = delete;
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    void setTexture(const std::string& name, const TextureUnitId texUnit, const uint32_t texId,
        const TextureTargetType type) const;
    inline void handleNotFoundLocation(const std::string& name) const;

private:
    uint32_t shaderId_{0};
    Logger log_;
    static uint32_t boundShaderId_;
};
} // namespace msgui