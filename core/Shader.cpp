#include "Shader.hpp"

#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace msgui
{
// ---- Static init ---- //
uint32_t Shader::boundShaderId_ = 0;

Shader::Shader(const uint32_t shaderId, const std::string& shaderName)
    : shaderId_(shaderId)
    , log_("Shader(" + shaderName + " = " + std::to_string(shaderId_) + ")")
{}

Shader& Shader::operator=(Shader&& other)
{
    shaderId_ = other.shaderId_;
    other.shaderId_ = 0;
    // log_.info("Moved assign");
    return *this;
}

Shader::~Shader()
{
    glDeleteProgram(shaderId_);
    // log_.info("Deleted shader");
}

// ---- Normal ---- //
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

// ---- Setters ---- //
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

// ---- Getters ---- //
uint32_t Shader::getShaderId() const
{
    return shaderId_;
}

// ---- Setters Private ---- //
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

    // Shader needs texture unit location in range from [0..maxUnits], not from [GL_TEXTURE0..maxGL_TEXTURE]
    setInt(name, texUnit - GL_TEXTURE0);
    glActiveTexture(texUnit);

    glBindTexture(type, texId);
}

// ---- Normal Private ---- //
inline void Shader::handleNotFoundLocation(const std::string& name) const
{
    log_.errorLn("Uniform \"%s\" not found", name.c_str());
}
} // namespace msgui