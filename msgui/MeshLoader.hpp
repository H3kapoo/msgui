#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Mesh.hpp"
#include "Logger.hpp"

namespace msgui
{
/* Class that loads a mesh and stores it uniquely accross windows */
class MeshLoader
{
/* Custom key for a simple quad */
const static constexpr std::string INTERNAL_QUAD_KEY{"//iQuadMesh"};

private:
    enum BufferType : uint8_t { STATIC, DYNAMIC, STREAM };

    struct Layer
    {
        std::string name;
        uint32_t index{0};
        uint32_t count{0};
    };

public:
    /**
        Specifically load the quad mesh type.

        @return Mesh pointer
    */
    static Mesh* loadQuad();

private:
    /* Cannot be copied or moved */
    MeshLoader() = default;
    ~MeshLoader();
    MeshLoader(const MeshLoader&);
    MeshLoader(MeshLoader&&);
    MeshLoader& operator=(const MeshLoader&);
    MeshLoader& operator=(MeshLoader&&);

    Mesh loadInternalQuad();
    void setDataBuffer(uint32_t& vboId, std::vector<float>& data, const BufferType drawType = MeshLoader::STATIC);
    void setElementBuffer(uint32_t& eboId, std::vector<uint32_t>& data, const BufferType drawType = MeshLoader::STATIC);
    void addVertexAttribLayer(std::vector<Layer>& layers, const uint32_t vertexAttribIndex, const uint32_t dataTypeCount,
        const std::string& layerName);
    void buildAttribLayers(std::vector<Layer>& layers);
    uint32_t convertDrawType(const BufferType drawType) const;

    static MeshLoader& get();

private:
    static Logger log_;
    static std::unordered_map<std::string, Mesh*> meshPathToObject_;
};
} // namespace msgui