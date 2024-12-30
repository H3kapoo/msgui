#include "Debug.hpp"
#include "Logger.hpp"

#include <GLFW/glfw3.h>
#include <string>

namespace msgui
{
Debug& Debug::get()
{
    static Debug instance;
    return instance;
}

Debug::Debug()
    : log_("Debug")
{
    if (glfwGetCurrentContext() == nullptr)
    {
        log_.errorLn("No active GLFW context found!");
        return;
    }

    int32_t contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    if (!(contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
        log_.warnLn("Debug messages context object created without window suppoting it!");
        return;
    }


    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(Debug::debugCallback, &this->log_);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    log_.debugLn("Context for debug messages enabled");

    log_.debugLn("GPU Details: %s %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));

    int32_t majorVersion{0}, minorVersion{0};
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    log_.debugLn("OpenGL version: %d.%d | %s", majorVersion, minorVersion, glGetString(GL_VERSION));

    int32_t maxTextureUnits{0};
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    log_.debugLn("Max texture units: %d", maxTextureUnits);

    float maxAnisotropicFiltering{0.0f};
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropicFiltering);
    log_.debugLn("Max AA supported: %f", maxAnisotropicFiltering);
}

void Debug::enableWireframe()
{
    log_.debugLn("Enabled wireframe mode");
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Debug::disableWireframe()
{
    log_.debugLn("Disabled wireframe mode");
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Debug::debugCallback(GLenum eSource, GLenum eType, unsigned int id, GLenum eSeverity, GLsizei,
    const char *message, const void *userParam)
{

#define E_TO_CASE(var, s)\
    case s:\
    {\
        var = #s;\
        break;\
    }\

    std::string source{"UNKNOWN"};
    std::string type{"UNKNOWN"};
    std::string severity{"UNKNOWN"};
    switch (eSource)
    {
        E_TO_CASE(source, GL_DEBUG_SOURCE_API_ARB)
        E_TO_CASE(source, GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
        E_TO_CASE(source, GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
        E_TO_CASE(source, GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
        E_TO_CASE(source, GL_DEBUG_SOURCE_APPLICATION_ARB)
        E_TO_CASE(source, GL_DEBUG_SOURCE_OTHER_ARB)
    }

    switch (eType)
    {
        E_TO_CASE(type, GL_DEBUG_TYPE_ERROR_ARB)
        E_TO_CASE(type, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
        E_TO_CASE(type, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
        E_TO_CASE(type, GL_DEBUG_TYPE_PORTABILITY_ARB)
        E_TO_CASE(type, GL_DEBUG_TYPE_PERFORMANCE_ARB)
        E_TO_CASE(type, GL_DEBUG_TYPE_OTHER_ARB)
    }

    switch (eSeverity)
    {
        E_TO_CASE(severity, GL_DEBUG_SEVERITY_HIGH_ARB)
        E_TO_CASE(severity, GL_DEBUG_SEVERITY_MEDIUM_ARB)
        E_TO_CASE(severity, GL_DEBUG_SEVERITY_LOW_ARB)
    }

    const auto log_ = static_cast<const Logger*>(userParam);
    log_->errorLn("---------(id:%d)---------", id);
    log_->errorLn("Source  : %s", source.c_str());
    log_->errorLn("Type    : %s", type.c_str());
    log_->errorLn("Severity: %s", severity.c_str());
    log_->errorLn("Message : %s", message);
    log_->errorLn("-------------------------", id);

#undef E_TO_CASE
}
} // namespace msgui