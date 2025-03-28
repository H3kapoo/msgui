#include "Logger.hpp"

#include <stdio.h>
#include <cstdarg>

#include <GLFW/glfw3.h>

namespace msgui
{
uint8_t Logger::allowedLevels = Level::ALL;

Logger::Logger(const std::string name, const std::string&)
    : name_{name}
{}

// ---- Normal ---- //
void Logger::raw(const char* format, ...) const
{
    va_list vList;
    va_start(vList, format);
    vfprintf(stdout, format, vList);
    fflush(stdout);
    va_end(vList);
}

void Logger::info(const char* format, ...) const
{
    if (!(Level::INFO & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][INF][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    fflush(stdout);
    va_end(vList);
}

void Logger::infoLn(const char* format, ...) const
{
    if (!(Level::INFO & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][INF][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    fprintf(stdout, "\n");
    va_end(vList);
}

void Logger::debug(const char* format, ...) const
{
    if (!(Level::DEBUG & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][DBG][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    va_end(vList);
}

void Logger::debugLn(const char* format, ...) const
{
    if (!(Level::DEBUG & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][DBG][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    fprintf(stdout, "\n");
    va_end(vList);
}

void Logger::warn(const char* format, ...) const
{
    if (!(Level::WARNING & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][WRN][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    va_end(vList);
}

void Logger::warnLn(const char* format, ...) const
{
    if (!(Level::WARNING & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][WRN][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    fprintf(stdout, "\n");
    va_end(vList);
}

void Logger::error(const char* format, ...) const
{
    if (!(Level::ERROR & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stderr, "[%lf][ERR][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stderr, format, vList);
    va_end(vList);
}

void Logger::errorLn(const char* format, ...) const
{
    if (!(Level::ERROR & allowedLevels))
    {
        return;
    }

    va_list vList;
    va_start(vList, format);
    fprintf(stdout, "[%lf][ERR][%s] ", glfwGetTime(), name_.c_str());
    vfprintf(stdout, format, vList);
    fprintf(stdout, "\n");
    fflush(stdout);
    va_end(vList);
}

// ---- Getters ---- //
const std::string& Logger::getName() const
{
    return name_;
}

// ---- Static Setters ---- //
void Logger::setLevels(const uint8_t level)
{
    allowedLevels |= level;
}

void Logger::unsetLevels(const uint8_t level)
{
    allowedLevels &= ~level;
}
} // namespace msgui