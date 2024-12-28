#pragma once

#include <cstdint>

#include "Logger.hpp"

namespace msgui
{
class Mesh
{
public:
    explicit Mesh(uint32_t vaoId, uint32_t eboId, uint32_t vboId);
    Mesh(Mesh&& other);
    ~Mesh();

    void bind() const;
    void unbind() const;
    uint32_t getVaoId() const;
    uint32_t getVaoId();

private:
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;


private:
    Logger log_;
    uint32_t vaoId_{0};
    uint32_t eboId_{0};
    uint32_t vboId_{0};
};
} // namespace msgui