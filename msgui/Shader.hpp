#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Logger.hpp"

namespace msgui
{
using TextureUnitId = uint32_t;
using TextureTargetType = uint32_t;

/* Class holding data about a shader */
class Shader
{
public:
    /**
        Create a shader from given data.

        @param shaderId Id of the shader
        @param shaderName Name of the shader
     */
    explicit Shader(const uint32_t shaderId, const std::string& shaderName);
    ~Shader();
    Shader& operator=(Shader&& other);

    /**
        Binds the current shader id.
    */
    void bind() const;

    /**
        Unbinds the current shader id.
    */
    void unbind() const;

    /**
        Set texture 1D uniform inside the shader.

        @param name Name of the folder
        @param texUnit Texture unit to bind texture to
        @param texId Id of the texture
    */
    void setTexture1D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;

    /**
        Set texture 2D uniform inside the shader.

        @param name Name of the folder
        @param texUnit Texture unit to bind texture to
        @param texId Id of the texture
    */
    void setTexture2D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;

    /**
        Set texture 3D uniform inside the shader.

        @param name Name of the folder
        @param texUnit Texture unit to bind texture to
        @param texId Id of the texture
    */
    void setTexture3D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;

    /**
        Set texture array 1D uniform inside the shader.

        @param name Name of the folder
        @param texUnit Texture unit to bind texture to
        @param texId Id of the texture
    */
    void setTexture1DArray(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;

    /**
        Set texture array 2D uniform inside the shader.

        @param name Name of the folder
        @param texUnit Texture unit to bind texture to
        @param texId Id of the texture
    */
    void setTexture2DArray(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const;

    /**
        Set integer uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setInt(const std::string& name, const int32_t value) const;

    /**
        Set 2D integer vector uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setVec2i(const std::string& name, const glm::ivec2& value) const;

    /**
        Set 2D float vector uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setVec2f(const std::string& name, const glm::vec2& value) const;

    /**
        Set 3D float vector uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setVec3f(const std::string& name, const glm::vec3& value) const;

    /**
        Set 4D float vector uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setVec4f(const std::string& name, const glm::vec4& value) const;

    /**
        Set 4x4 matrix uniform inside the shader.

        @param name Name of the folder
        @param value Value to be set
    */
    void setMat4f(const std::string& name, const glm::mat4& value) const;

    /**
        Get the current shader id.

        @return Shader id
    */
    uint32_t getShaderId() const;

private:
    /* Cannot be copied and kinda moved */
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