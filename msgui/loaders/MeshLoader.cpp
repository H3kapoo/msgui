#include "MeshLoader.hpp"

#include <algorithm>

#include "GL/glew.h"

namespace msgui::loaders
{

Logger MeshLoader::log_ = {"MeshLoader(null)"};
std::unordered_map<std::string, Mesh*> MeshLoader::meshPathToObject_ = {};

MeshLoader::~MeshLoader()
{
    for (const auto&[key, value] : meshPathToObject_)
    {
        delete value;
    }

    log_ = Logger("MeshLoader(ALL)");
    log_.infoLn("Unloaded all meshes!");
}

Mesh* MeshLoader::loadQuad()
{
    if (meshPathToObject_.count(INTERNAL_QUAD_KEY))
    {
        return meshPathToObject_.at(INTERNAL_QUAD_KEY);
    }

    log_ = Logger("MeshLoader(Quad)");

    Mesh* meshPtr = new Mesh(get().loadInternalQuad());
    meshPathToObject_[INTERNAL_QUAD_KEY] = meshPtr;

    log_.infoLn("Loaded!");
    return meshPathToObject_.at(INTERNAL_QUAD_KEY);
}

Mesh MeshLoader::loadInternalQuad()
{
    static std::vector<float> vertices = {
         // positions        // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f,  0.0f, 0.0f,  1.0f, 0.0f, // bottom right
         0.0f,  0.0f, 0.0f,  0.0f, 0.0f, // bottom left
         0.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left 
    };

    static std::vector<uint32_t> indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    uint32_t vaoId, eboId, vboId;
    std::vector<Layer> layers;

    glCreateVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    setDataBuffer(vboId, vertices, BufferType::STATIC);
    setElementBuffer(eboId, indices, BufferType::STATIC);
    addVertexAttribLayer(layers, 0, 3, "Position");
    addVertexAttribLayer(layers, 1, 2, "TextureUV");
    buildAttribLayers(layers);

    return Mesh(vaoId, eboId, vboId);
}

void MeshLoader::setDataBuffer(uint32_t& vboId, std::vector<float>& data, const BufferType drawType)
{
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER,  vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(),
        convertDrawType(drawType));
}

void MeshLoader::setElementBuffer(uint32_t& eboId, std::vector<uint32_t>& data, const BufferType drawType)
{
    glGenBuffers(1, &eboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(float) * data.size(), data.data(),
        convertDrawType(drawType));
}

void MeshLoader::addVertexAttribLayer(std::vector<Layer>& layers, const uint32_t vertexAttribIndex, const uint32_t dataTypeCount,
    const std::string& layerName)
{
    layers.emplace_back(Layer{layerName, vertexAttribIndex, dataTypeCount});
}

void MeshLoader::buildAttribLayers(std::vector<Layer>& layers)
{
    std::sort(layers.begin(), layers.end(), [](const Layer& lhs, const Layer& rhs)
    {
        return lhs.index < rhs.index;
    });

    uint32_t stride{0};
    for (const auto& layer : layers)
    {
        stride += layer.count;
    }
    stride *=  sizeof(float);

    uint64_t offset{0};
    for (const auto& layer : layers)
    {
        glVertexAttribPointer(layer.index, layer.count, GL_FLOAT, false, stride, (void*)offset);
        glEnableVertexAttribArray(layer.index);
        offset += layer.count * sizeof(float);
    }
}

uint32_t MeshLoader::convertDrawType(const BufferType drawType) const
{
    switch (drawType)
    {
        case BufferType::STATIC:
            return GL_STATIC_DRAW;
        case BufferType::DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case BufferType::STREAM:
            return GL_STREAM_DRAW;
    }

    log_.error("Invalid draw type value: %d", drawType);
    return 0;
}

MeshLoader& MeshLoader::get()
{
    static MeshLoader instance;
    return instance;
}
} // namespace msgui::loaders