#include "Mesh.hpp"

#include "GL/glew.h"

namespace msgui
{
Mesh::Mesh(uint32_t vaoId, uint32_t eboId, uint32_t vboId)
    : log_("Mesh (" + std::to_string(vaoId) +")")
    , vaoId_(vaoId)
    , eboId_(eboId)
    , vboId_(vboId)
{
    log_.infoLn("Created!");
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &eboId_);
    glDeleteBuffers(1, &vboId_);
    glDeleteVertexArrays(1, &vaoId_);
    log_.infoLn("Deleted vertex array");
}

void Mesh::bind() const
{
    glBindVertexArray(vaoId_);
}

void Mesh::unbind() const
{
   glBindVertexArray(0);
}

uint32_t Mesh::getVaoId() const
{
    return vaoId_;
}

uint32_t Mesh::getVaoId()
{
    return vaoId_;
}
} // namespace msgui