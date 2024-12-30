#include "ShaderLoader.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>

namespace msgui
{
// ---- Static Init ---- //
Logger ShaderLoader::log_ = {"ShaderLoader(null)"};
std::unordered_map<std::string, Shader*> ShaderLoader::shaderPathToObject_ = {};

ShaderLoader::~ShaderLoader()
{
    for (const auto&[key, value] : shaderPathToObject_)
    {
        delete value;
    }

    log_ = Logger("ShaderLoader(ALL)");
    log_.infoLn("Unloaded all shader programs!");
}

// ---- Statics ---- //
Shader* ShaderLoader::load(const std::string& shaderPath)
{
    if (shaderPathToObject_.count(shaderPath))
    {
        return shaderPathToObject_.at(shaderPath);
    }

    log_ = Logger("ShaderLoader(" + shaderPath + ")");

    Shader* shaderPtr = new Shader(get().loadInternal(shaderPath), shaderPath);
    shaderPathToObject_[shaderPath] = shaderPtr;

    if (shaderPtr->getShaderId() != 0)
    {
        log_.infoLn("Loaded with id %d!", shaderPtr->getShaderId());
    }
    else
    {
        log_.error("Load failed! Zero program will be loaded.");
    }

    return shaderPathToObject_.at(shaderPath);
}

void ShaderLoader::reload(const std::string& shaderPath)
{
    log_ = Logger("ShaderLoader(" + shaderPath + ")");

    if (!shaderPathToObject_.count(shaderPath))
    {
        log_.warn("Cannot reload not loaded shader!");
        return;
    }


    if (uint32_t shaderId = get().loadInternal(shaderPath); shaderId != 0)
    {
        *shaderPathToObject_[shaderPath] = Shader(get().loadInternal(shaderPath), shaderPath);
        log_.infoLn("Reloaded!");
    }
    else
    {
        log_.error("Reload failed! Keeping previous program.");
    }
}

// ---- Normal Private ---- //
uint32_t ShaderLoader::loadInternal(const std::string& shaderPath)
{
    std::ifstream shaderFile(shaderPath);
    if (!shaderFile)
    {
        log_.error("Could not open shader file at %s\n", shaderPath.c_str());
        return 0;
    }

    std::stringstream stream;
    stream << shaderFile.rdbuf();
    std::string content = stream.str();
    shaderFile.close();

    const size_t fragCutoff = content.find("/// frag ///\n"); /* WRN: LF ending handled only */

    std::string vertData{content.begin(), content.begin() + fragCutoff};
    std::string fragData{content.begin() + fragCutoff, content.end()};

    uint32_t shaderId = loadInternal(vertData, fragData);
    return shaderId;
}

uint32_t ShaderLoader::loadInternal(const std::string& vertCode, const std::string& fragCode)
{
    uint32_t shaderId = linkShaders(
        compileShaderData(vertCode, GL_VERTEX_SHADER),
        compileShaderData(fragCode, GL_FRAGMENT_SHADER));

    return shaderId;
}

uint32_t ShaderLoader::linkShaders(int vertShaderId, int fragShaderId)
{
    if (vertShaderId == 0 || fragShaderId == 0) { return 0; }

    auto shaderId = glCreateProgram();

    glAttachShader(shaderId, vertShaderId);
    glAttachShader(shaderId, fragShaderId);
    glLinkProgram(shaderId);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
        log_.error("Could not link program:\n\t%s\n", infoLog);
        return 0;
    }

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return shaderId;
}

uint32_t ShaderLoader::compileShaderData(const std::string& data, const ShaderPartType shaderType)
{
    const char* src = data.c_str();
    uint32_t shaderPart = glCreateShader(shaderType);
    glShaderSource(shaderPart, 1, &src, NULL);
    glCompileShader(shaderPart);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderPart, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderPart, 512, NULL, infoLog);
        std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAG";
        log_.error("Compile failed for shader %s because:\n\t%s", type.c_str(), infoLog);
        return 0;
    }

    return shaderPart;
}

// ---- Statics Private ---- //
ShaderLoader& ShaderLoader::get()
{
    static ShaderLoader instance = ShaderLoader();
    return instance;
}
} // namesapce msgui