#pragma once

#include "export.hh"

#include <commons/math/vec2.hh>
#include <initializer_list>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <array>

namespace glr
{
  struct QuadUVs
  {
    vec2<float> upperLeft = {};
    vec2<float> lowerLeft = {};
    vec2<float> upperRight = {};
    vec2<float> lowerRight = {};
  };
  
  //TODO support index buffer
  struct Mesh
  {
    Mesh() = default;
    GLRENDER_API ~Mesh();
    
    //Base constructors, all others should be able to delegate to these
    GLRENDER_API Mesh(float const *verts, size_t vertsSize);
    GLRENDER_API Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize);
    GLRENDER_API Mesh(float const *verts, size_t vertsSize, float const *uvs, size_t uvsSize, float const *normals, size_t normalsSize);
    
    //Values
    GLRENDER_API explicit Mesh(std::vector<float> const &verts);
    GLRENDER_API Mesh(std::vector<float> const &verts, std::vector<float> const &uvs);
    GLRENDER_API Mesh(std::vector<float> const &verts, std::vector<float> const &uvs, std::vector<float> const &normals);
    GLRENDER_API Mesh(std::initializer_list<float> const &verts);
    GLRENDER_API Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs);
    GLRENDER_API Mesh(std::initializer_list<float> const &verts, std::initializer_list<float> const &uvs, std::initializer_list<float> const &normals);
    
    template <size_t N> GLRENDER_API explicit Mesh(std::array<float, N> const &verts) : Mesh(verts.data(), verts.size())
    {}
    template <size_t N> GLRENDER_API Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size())
    {}
    template <size_t N> GLRENDER_API Mesh(std::array<float, N> const &verts, std::array<float, N> const &uvs, std::array<float, N> const &normals) : Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size())
    {}
    
    //Raw data read out of files
    GLRENDER_API explicit Mesh(std::vector<uint8_t> const &verts);
    GLRENDER_API Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs);
    GLRENDER_API Mesh(std::vector<uint8_t> const &verts, std::vector<uint8_t> const &uvs, std::vector<uint8_t> const &normals);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> const &verts, std::initializer_list<uint8_t> const &uvs, std::initializer_list<uint8_t> const &normals);
    
    template <size_t N> GLRENDER_API explicit Mesh(std::array<uint8_t, N> const &verts) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4)
    {}
    template <size_t N> GLRENDER_API Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4, reinterpret_cast<float const *>(uvs.data()), uvs.size() / 4)
    {}
    template <size_t N> GLRENDER_API Mesh(std::array<uint8_t, N> const &verts, std::array<uint8_t, N> const &uvs, std::array<uint8_t, N> const &normals) : Mesh(reinterpret_cast<float const *>(verts.data()), verts.size() / 4, reinterpret_cast<float const *>(uvs.data()), uvs.size() / 4, reinterpret_cast<float const *>(normals.data()), normals.size() / 4)
    {}
    
    Mesh(Mesh const &copyFrom) = delete;
    Mesh& operator=(Mesh const &copyFrom) = delete;
    GLRENDER_API Mesh(Mesh &&moveFrom) noexcept;
    GLRENDER_API Mesh& operator=(Mesh &&moveFrom) noexcept;
    
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use() const;
    
    uint32_t vao = std::numeric_limits<uint32_t>::max();
    uint32_t vboV = std::numeric_limits<uint32_t>::max();
    uint32_t vboU = std::numeric_limits<uint32_t>::max();
    uint32_t vboN = std::numeric_limits<uint32_t>::max();
    uint32_t vboI = std::numeric_limits<uint32_t>::max();
    size_t numVerts = 0;
    bool hasVerts = false;
    bool hasUVs = false;
    bool hasNormals = false;
    
    private:
    int32_t vertexStride = 3 * sizeof(float);
    int32_t uvStride = 2 * sizeof(float);
    int32_t normalStride = 3 * sizeof(float);
    bool init = false;
  };
}
