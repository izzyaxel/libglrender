#pragma once

#include "export.hh"
#include "glrUtil.hh"

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
    bool operator==(const QuadUVs& other) const
    {
      return this->upperLeft == other.upperLeft && this->lowerLeft == other.lowerLeft &&
      this->upperRight == other.upperRight && this->lowerRight == other.lowerRight;
    }
    
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
    GLRENDER_API Mesh(const float* verts, size_t vertsSize);
    GLRENDER_API Mesh(const float* verts, size_t vertsSize, const float* uvs, size_t uvsSize);
    GLRENDER_API Mesh(const float* verts, size_t vertsSize, const float* uvs, size_t uvsSize, const float* normals, size_t normalsSize);
    
    //Values
    GLRENDER_API explicit Mesh(const std::vector<float>& verts);
    GLRENDER_API Mesh(const std::vector<float>& verts, const std::vector<float>& uvs);
    GLRENDER_API Mesh(const std::vector<float>& verts, const std::vector<float>& uvs, const std::vector<float>& normals);
    GLRENDER_API Mesh(std::initializer_list<float> verts);
    GLRENDER_API Mesh(std::initializer_list<float> verts, std::initializer_list<float> uvs);
    GLRENDER_API Mesh(std::initializer_list<float> verts, std::initializer_list<float> uvs, std::initializer_list<float> normals);
    
    template <size_t N> GLRENDER_API explicit Mesh(const std::array<float, N>& verts) :
    Mesh(verts.data(), verts.size())
    {}
    template <size_t N, size_t O> GLRENDER_API Mesh(const std::array<float, N>& verts, const std::array<float, O>& uvs) :
    Mesh(verts.data(), verts.size(), uvs.data(), uvs.size())
    {}
    template <size_t N, size_t O, size_t P> GLRENDER_API Mesh(const std::array<float, N>& verts, const std::array<float, O>& uvs, const std::array<float, P>& normals) :
    Mesh(verts.data(), verts.size(), uvs.data(), uvs.size(), normals.data(), normals.size())
    {}
    
    //Raw data read out of files
    GLRENDER_API explicit Mesh(const std::vector<uint8_t>& verts);
    GLRENDER_API Mesh(const std::vector<uint8_t>& verts, const std::vector<uint8_t>& uvs);
    GLRENDER_API Mesh(const std::vector<uint8_t>& verts, const std::vector<uint8_t>& uvs, const std::vector<uint8_t>& normals);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> verts);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> verts, std::initializer_list<uint8_t> uvs);
    GLRENDER_API Mesh(std::initializer_list<uint8_t> verts, std::initializer_list<uint8_t> uvs, std::initializer_list<uint8_t> normals);
    
    template <size_t N> GLRENDER_API explicit Mesh(const std::array<uint8_t, N>& verts) :
    Mesh(reinterpret_cast<const float*>(verts.data()), verts.size() / 4)
    {}
    template <size_t N, size_t O> GLRENDER_API Mesh(const std::array<uint8_t, N>& verts, const std::array<uint8_t, O>& uvs) :
    Mesh(reinterpret_cast<const float*>(verts.data()), verts.size() / 4, reinterpret_cast<const float*>(uvs.data()), uvs.size() / 4)
    {}
    template <size_t N, size_t O, size_t P> GLRENDER_API Mesh(const std::array<uint8_t, N>& verts, const std::array<uint8_t, O>& uvs, const std::array<uint8_t, P>& normals) :
    Mesh(reinterpret_cast<const float*>(verts.data()), verts.size() / 4, reinterpret_cast<const float*>(uvs.data()), uvs.size() / 4, reinterpret_cast<const float*>(normals.data()), normals.size() / 4)
    {}
    
    Mesh(const Mesh& copyFrom) = delete;
    Mesh& operator=(const Mesh& copyFrom) = delete;
    GLRENDER_API Mesh(Mesh&& moveFrom) noexcept;
    GLRENDER_API Mesh& operator=(Mesh&& moveFrom) noexcept;
    
    [[nodiscard]] GLRENDER_API bool isValid() const;
    [[nodiscard]] GLRENDER_API bool exists() const;
    GLRENDER_API void reset();
    GLRENDER_API void use() const;
    
    uint32_t vao =  INVALID_HANDLE;
    uint32_t vboV = INVALID_HANDLE;
    uint32_t vboU = INVALID_HANDLE;
    uint32_t vboN = INVALID_HANDLE;
    uint32_t vboI = INVALID_HANDLE;
    size_t numVerts = 0;
    bool hasVerts = false;
    bool hasUVs = false;
    bool hasNormals = false;
    
    private:
    int32_t vertexStride = 3 * sizeof(float);
    int32_t uvStride =     2 * sizeof(float);
    int32_t normalStride = 3 * sizeof(float);
    bool init = false;
  };
}
