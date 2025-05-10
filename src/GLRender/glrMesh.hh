#pragma once

#include "export.hh"
#include "glrUtil.hh"
#include "glrEnums.hh"
#include "glrLogging.hh"

#include <commons/math/vec2.hh>
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

    //It's the user's responsibility to keep these buffers lined up with each other
    //Ie: you should be adding UVs and/or normals that correspond to all the positions you've added
    //If this isn't done, odd effects may occur

    /// Add vertex positions to the OpenGL buffer, can be called multiple times to append data to the end of the positions buffer
    /// @param positions An array of vertex positions to add to the OpenGL buffer
    /// @param positionsSize How many elements are in the positions array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addPositions(const float* positions, size_t positionsSize, const LoggingCallback& callback = nullptr);

    /// Add vertex uv coordinates to the OpenGL buffer, can be called multiple times to append data to the end of the uv coordinates buffer
    /// @param uvs An array of vertex uv coordinates to add to the OpenGL buffer
    /// @param uvsSize How many elements are in the uvs array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addUVs(const float* uvs, size_t uvsSize, const LoggingCallback& callback = nullptr);

    /// Add vertex normals to the OpenGL buffer, can be called multiple times to append data to the end of the normals buffer
    /// @param normals An array of vertex normals to add to the OpenGL buffer
    /// @param normalsSize How many elements are in the normals array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addNormals(const float* normals, size_t normalsSize, const LoggingCallback& callback = nullptr);

    //TODO add vertex colors

    /// Interleave all the vertex data upload it to the GPU, and lock the Mesh
    /// @param callback The error/warning/info handling callback, this is given information about what occured when something goes wrong
    GLRENDER_API void finalize(const LoggingCallback& callback = nullptr);

    /// Bind the mesh's vertex array for use after it's been finalized
    GLRENDER_API void use() const;

    GLRENDER_API bool isFinalized() const;
    
    uint32_t vertexArrayHandle = INVALID_HANDLE;
    uint32_t vertexBufferHandle = INVALID_HANDLE;
    
    size_t numVerts = 0;

    bool hasPositions = false;
    bool hasUVs = false;
    bool hasNormals = false;

    GLDrawType type = GLDrawType::STATIC;
    GLDrawMode drawMode = GLDrawMode::TRIS;

    constexpr static int32_t POSITION_STRIDE = 3 * sizeof(float);
    constexpr static int32_t NORMAL_STRIDE = 3 * sizeof(float);
    constexpr static int32_t UV_STRIDE =     2 * sizeof(float);
    
    constexpr static int32_t POSITION_BINDING_POINT = 0;
    constexpr static int32_t NORMAL_BINDING_POINT = 1;
    constexpr static int32_t UV_BINDING_POINT = 2;
    
    private:
    int getGLDrawType() const;

    std::vector<float> positions{};
    std::vector<float> uvs{};
    std::vector<float> normals{};

    bool finalized = false;
  };
}
