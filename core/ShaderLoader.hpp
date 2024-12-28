#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>

#include "Shader.hpp"
#include "Logger.hpp"

namespace msgui
{
using ShaderPartType = uint32_t;

class ShaderLoader
{
public:
    static Shader* load(const std::string& shaderPath);
    static void reload(const std::string& shaderPath);

private:
    ShaderLoader() = default;
    ~ShaderLoader();
    ShaderLoader(const ShaderLoader&);
    ShaderLoader(ShaderLoader&&);
    ShaderLoader& operator=(const ShaderLoader&);
    ShaderLoader& operator=(ShaderLoader&&);

    static ShaderLoader& get();

    uint32_t loadInternal(const std::string& shaderPath);
    uint32_t loadInternal(const std::string& vertCode, const std::string& fragCode);
    uint32_t linkShaders(int vertShaderId, int fragShaderId);
    uint32_t compileShaderData(const std::string& data, const ShaderPartType shaderType);

private:
    static Logger log_;

    static std::unordered_map<std::string, Shader*> shaderPathToObject_;
};
} // namespace msgui