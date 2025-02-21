#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Logger.hpp"

namespace msgui
{
using ShaderPartType = uint32_t;

/* Class that loads a shader and stores it uniquely accross windows */
class ShaderLoader
{
public:
    /**
        Load a shader glsl file from path.
        Note: Vertex and Fragment shader are combined inside one file, not separated.

        @param shaderPath Path to load shader from
        @return Shader pointer
    */
    static Shader* loadShader(const std::string& shaderPath);

    /**
        Runtime reload a shader given a path to it.

        @param shaderPath Path to load shader from
        @return Shader pointer
    */
    static void reload(const std::string& shaderPath);

private:
    /* Cannot be copied or moved */
    ShaderLoader() = default;
    ~ShaderLoader();
    ShaderLoader(const ShaderLoader&);
    ShaderLoader(ShaderLoader&&);
    ShaderLoader& operator=(const ShaderLoader&);
    ShaderLoader& operator=(ShaderLoader&&);

    uint32_t loadInternal(const std::string& shaderPath);
    uint32_t loadInternal(const std::string& vertCode, const std::string& fragCode);
    uint32_t linkShaders(int vertShaderId, int fragShaderId);
    uint32_t compileShaderData(const std::string& data, const ShaderPartType shaderType);

    static ShaderLoader& get();

private:
    static Logger log_;
    static std::unordered_map<std::string, Shader*> shaderPathToObject_;
};
} // namespace msgui