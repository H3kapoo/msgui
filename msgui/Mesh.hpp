#pragma once

#include <cstdint>

#include "Logger.hpp"

namespace msgui
{
/* Class holding data about a mesh */
class Mesh
{
public:
    /**
        Create a mesh from given data indexes.

        @param vaoId Vertex array object Id of the mesh
        @param eboId Element buffer object Id of the mesh
        @param vboId Vertex buffer object Id of the mesh
     */
    explicit Mesh(uint32_t vaoId, uint32_t eboId, uint32_t vboId);
    Mesh(Mesh&& other);
    ~Mesh();

    /**
        Binds the current mesh vao.
    */
    void bind() const;

    /**
        Unbinds the current mesh vao.
    */
    void unbind() const;

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

    static uint32_t boundVao_;
};
} // namespace msgui