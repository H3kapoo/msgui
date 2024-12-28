#pragma once

#include <GL/glew.h>

#include "Logger.hpp"

namespace msgui
{
class Debug
{
public:
    static Debug& get();

    void enableWireframe();
    void disableWireframe();
private:
    Debug();
    Debug(const Debug&);
    Debug& operator=(const Debug&);

    static void debugCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, 
        const char *message, const void *userParam);

private:
    Logger log_;
};
} // namespace msgui