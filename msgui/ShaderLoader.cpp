#include "ShaderLoader.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <future>

#include <GLFW/glfw3.h>

#include "msgui/BELoadingQueue.hpp"

namespace msgui
{
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

Shader* ShaderLoader::loadShader(const std::string& shaderPath)
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
        log_.errorLn("Load failed! Zero program will be loaded.");
    }

    return shaderPathToObject_.at(shaderPath);
}

void ShaderLoader::reload(const std::string& shaderPath)
{
    log_ = Logger("ShaderLoader(" + shaderPath + ")");

    if (!shaderPathToObject_.count(shaderPath))
    {
        log_.warnLn("Cannot reload not loaded shader!");
        return;
    }

    // TODO: Previous shader needs to be deleted, consumes memory.
    if (uint32_t shaderId = get().loadInternal(shaderPath); shaderId != 0)
    {
        *shaderPathToObject_[shaderPath] = Shader(get().loadInternal(shaderPath), shaderPath);
        log_.infoLn("Reloaded %d!",  shaderPathToObject_[shaderPath]->getShaderId());
    }
    else
    {
        log_.errorLn("Reload failed! Keeping previous program.");
    }
}

uint32_t ShaderLoader::loadInternal(const std::string& shaderPath)
{
    std::packaged_task<uint32_t()> task([shaderPath]() -> uint32_t
    {
        ShaderLoader& instance = get();

        std::ifstream shaderFile(shaderPath);
        if (!shaderFile)
        {
            log_.errorLn("Could not open shader file at %s", shaderPath.c_str());
            return 0;
        }

        std::stringstream stream;
        stream << shaderFile.rdbuf();
        std::string content = stream.str();
        shaderFile.close();

        const size_t fragCutoff = content.find("/// frag ///\n"); /* WRN: LF ending handled only */
        if (fragCutoff == std::string::npos)
        {
            log_.errorLn("Couldn't find FRAG start tag");
            return 0;
        }

        std::string vertData{content.begin(), content.begin() + fragCutoff};
        std::string fragData{content.begin() + fragCutoff, content.end()};

        uint32_t shaderId = instance.loadInternal(vertData, fragData);
        return shaderId;
    });

    auto futureTask = task.get_future();

    if (BELoadingQueue::get().isThisMainThread()) { task(); }
    /* This is not the main thread */
    else { BELoadingQueue::get().pushTask(std::move(task)); }

    return futureTask.get();
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
        log_.errorLn("Could not link program:\n\t%s\n", infoLog);
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
        log_.errorLn("Compile failed for shader %s because:\n\t%s", type.c_str(), infoLog);
        return 0;
    }

    return shaderPart;
}

ShaderLoader& ShaderLoader::get()
{
    static ShaderLoader instance = ShaderLoader();
    return instance;
}
} // namesapce msgui