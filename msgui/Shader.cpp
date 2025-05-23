#include "Shader.hpp"

#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace msgui
{
uint32_t Shader::boundShaderId_ = 0;

Shader::Shader(const uint32_t shaderId, const std::string& shaderName)
    : shaderId_(shaderId)
    , log_("Shader(" + shaderName + " = " + std::to_string(shaderId_) + ")")
{}

Shader& Shader::operator=(Shader&& other)
{
    /* Used for hot reloading code */
    shaderId_ = other.shaderId_;
    other.shaderId_ = 0;
    return *this;
}

Shader::~Shader()
{
    glDeleteProgram(shaderId_);
}

void Shader::bind() const
{
    if (shaderId_ == boundShaderId_) { return; }
    boundShaderId_ = shaderId_;
    glUseProgram(boundShaderId_);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::setTexture1D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const
{
    setTexture(name, texUnit, texId, GL_TEXTURE_1D);
}

void Shader::setTexture2D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const
{
    setTexture(name, texUnit, texId, GL_TEXTURE_2D);
}

void Shader::setTexture3D(const std::string& name, const TextureUnitId texUnit, const uint32_t texId) const
{
    setTexture(name, texUnit, texId, GL_TEXTURE_3D);
}

void Shader::setTexture1DArray(const std::string& name, const TextureUnitId texUnit,
    const uint32_t texId) const
{
    setTexture(name, texUnit, texId, GL_TEXTURE_1D_ARRAY);
}

void Shader::setTexture2DArray(const std::string& name, const TextureUnitId texUnit,
    const uint32_t texId) const
{
    setTexture(name, texUnit, texId, GL_TEXTURE_2D_ARRAY);
}

void Shader::setInt(const std::string& name, const int32_t value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }

    glUniform1i(loc, value);
}

void Shader::setIntv(const std::string& name, const std::vector<int32_t>& values) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    glUniform1iv(loc, values.size(), values.data());
}

void Shader::setVec2i(const std::string& name, const glm::ivec2& value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    glUniform2i(loc, value.x, value.y);
}

void Shader::setVec2f(const std::string& name, const glm::vec2& value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    glUniform2f(loc, value.x, value.y);
}

void Shader::setVec3f(const std::string& name, const glm::vec3& value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setVec4f(const std::string& name, const glm::vec4& value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Shader::setMat4f(const std::string& name, const glm::mat4& value) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    constexpr uint32_t transposeMatrix = GL_FALSE;
    glUniformMatrix4fv(loc, 1, transposeMatrix, glm::value_ptr(value));
}

void Shader::setMat4fv(const std::string& name, const std::vector<glm::mat4>& values) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }
    constexpr uint32_t transposeMatrix = GL_FALSE;
    glUniformMatrix4fv(loc, values.size(), transposeMatrix, glm::value_ptr(values[0]));
}

void Shader::setPartialMat4fv(const std::string& name, const int32_t startIdx,  const int32_t endIdx,
    const std::vector<glm::mat4>& values) const
{
    bind();

    int32_t loc = glGetUniformLocation(shaderId_, name.c_str());
    if (loc == -1)
    {
        return handleNotFoundLocation(name);
    }

    if (endIdx - startIdx <= 0)
    {
        log_.warnLn("Trying to set mat4v but the indices difference is zero or less than zero!");
        return;
    }

    constexpr uint32_t transposeMatrix = GL_FALSE;
    glUniformMatrix4fv(loc, endIdx - startIdx, transposeMatrix, glm::value_ptr(values[startIdx]));
}

uint32_t Shader::getShaderId() const
{
    return shaderId_;
}

void Shader::setTexture(const std::string& name, const TextureUnitId texUnit, const uint32_t texId,
    const TextureTargetType type) const
{
    bind();

    int32_t maxTextureUnitIds;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnitIds);
    if (texUnit + 1 > GL_TEXTURE0 + (uint32_t)maxTextureUnitIds)
    {
        log_.error("GPU not able to support more than %d texture units. Tried to use unit %d",
            maxTextureUnitIds, (int32_t)texUnit - (int32_t)GL_TEXTURE0);
        return;
    }

    /* Shader needs texture unit location in range from [0..maxUnits], not from [GL_TEXTURE0..maxGL_TEXTURE] */
    setInt(name, texUnit - GL_TEXTURE0);

    /* Active unit needs to be indeed [GL_TEXTURE0..maxGL_TEXTURE] */
    glActiveTexture(texUnit);

    glBindTexture(type, texId);
}

inline void Shader::handleNotFoundLocation(const std::string& name) const
{
    log_.errorLn("Uniform \"%s\" not found", name.c_str());
    exit(1);
}
} // namespace msgui