#pragma once

#include "export.hh"
#include "glrUtil.hh"
#include "glrEnums.hh"

#include <commons/math/vec2.hh>
#include <vector>
#include <cstdint>

namespace glr
{
  struct QuadUVs //TODO does this need to exist?
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

  //TODO indexed rendering of interleaved data
  struct Mesh
  {
    GLRENDER_API Mesh() = default;
    GLRENDER_API ~Mesh();
    
    Mesh(const Mesh& copyFrom) = delete;
    Mesh& operator=(const Mesh& copyFrom) = delete;
    GLRENDER_API Mesh(Mesh&& moveFrom) noexcept;
    GLRENDER_API Mesh& operator=(Mesh&& moveFrom) noexcept;

    GLRENDER_API void setDrawType(GLDrawType type);
    GLRENDER_API GLDrawType getDrawType() const;
    GLRENDER_API void setDrawMode(GLDrawMode mode);
    GLRENDER_API GLDrawMode getDrawMode() const;
    
    GLRENDER_API Mesh* addVerts(const float* verts, size_t vertsSize);
    GLRENDER_API Mesh* addUVs(const float* uvs, size_t uvsSize);
    GLRENDER_API Mesh* addNormals(const float* normals, size_t normalsSize);
    
    GLRENDER_API void finalize();
    GLRENDER_API void use() const;
    
    uint32_t vertArrayHandle = INVALID_HANDLE;
    uint32_t vertBufferHandle = INVALID_HANDLE;
    
    size_t numVerts = 0;
    
    bool hasVerts = false;
    bool hasUVs = false;
    bool hasNormals = false;
    
    private:
    int getGLDrawType() const;

    GLDrawType type = GLDrawType::STATIC;
    GLDrawMode mode = GLDrawMode::TRIS;

    const float* verts = nullptr;
    size_t vertsSize = 0;

    const float* uvs = nullptr;
    size_t uvsSize = 0;
    
    const float* normals = nullptr;
    size_t normalsSize = 0;
    
    static constexpr int32_t vertexStride = 3 * sizeof(float);
    static constexpr int32_t uvStride =     2 * sizeof(float);
    static constexpr int32_t normalStride = 3 * sizeof(float);
  };
}
