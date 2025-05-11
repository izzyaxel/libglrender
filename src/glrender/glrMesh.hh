#pragma once

#include "export.hh"
#include "glrUtil.hh"
#include "glrEnums.hh"
#include "glrLogging.hh"

#include <commons/math/vec2.hh>
#include <cstdint>

namespace glr
{
  struct QuadUVs //Used for atlas UVs
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
  //TODO dynamic changing of buffer data
  struct Mesh
  {
    GLRENDER_API Mesh() = default;
    GLRENDER_API ~Mesh();
    
    Mesh(const Mesh& copyFrom) = delete;
    Mesh& operator=(const Mesh& copyFrom) = delete;
    GLRENDER_API Mesh(Mesh&& moveFrom) noexcept;
    GLRENDER_API Mesh& operator=(Mesh&& moveFrom) noexcept;
    
    /// @param dimensions Set positions to 2D or 3D (2 or 3 coordinates per vertex, xy or xyz)
    GLRENDER_API void setPositionDimensions(GLDimensions dimensions);

    /// Add vertex positions to the OpenGL buffer, can be called multiple times to append data to the positions buffer
    /// @param positions An array of vertex positions to add to the OpenGL buffer
    /// @param positionsSize How many elements are in the positions array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addPositions(const float* positions, size_t positionsSize, const LoggingCallback& callback = nullptr);

    /// Add vertex uv coordinates to the OpenGL buffer, can be called multiple times to append data to the uv coordinates buffer
    /// @param uvs An array of vertex uv coordinates to add to the OpenGL buffer
    /// @param uvsSize How many elements are in the uvs array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addUVs(const float* uvs, size_t uvsSize, const LoggingCallback& callback = nullptr);

    /// Add vertex normals to the OpenGL buffer, can be called multiple times to append data to the normals buffer
    /// @param normals An array of vertex normals to add to the OpenGL buffer
    /// @param normalsSize How many elements are in the normals array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addNormals(const float* normals, size_t normalsSize, const LoggingCallback& callback = nullptr);

    /// Add vertex colors to the OpenGL buffer, can be called multiple times to append data to the colors buffer
    /// @param colors An array of vertex colors to add to the OpenGL buffer
    /// @param colorsSize How many elements are in the colors array
    /// @param callback The error/warning/info handling callback, this called when something goes wrong.  Nullable.
    GLRENDER_API Mesh* addColors(const float* colors, size_t colorsSize, const LoggingCallback& callback = nullptr);

    /// Interleave all the vertex data upload it to the GPU, and lock the Mesh
    /// @param callback The error/warning/info handling callback, this is given information about what occured when something goes wrong
    GLRENDER_API void finalize(const LoggingCallback& callback = nullptr);

    /// Bind the mesh's vertex array for use after it's been finalized
    GLRENDER_API void use() const;

    GLRENDER_API bool isFinalized() const;
    GLRENDER_API bool isIndexed() const;

    GLBufferType bufferType = GLBufferType::SEPARATE;
    GLDrawType drawType = GLDrawType::STATIC;
    GLDrawMode drawMode = GLDrawMode::TRIS;

    size_t numVerts = 0;

    int32_t positionBindingPoint = 0;
    int32_t normalBindingPoint = 2;
    int32_t uvBindingPoint = 1;
    int32_t colorBindingPoint = 3;

    //Whether to clear the buffer data from RAM after the Mesh is finalized
    bool retainBufferData = false;
    
    private:
    int getGLDrawType() const;

    uint32_t vertexArrayHandle = INVALID_HANDLE;
    uint32_t indexBufferHandle = INVALID_HANDLE;
    
    //Separate buffers
    uint32_t positionBufferHandle = INVALID_HANDLE;
    uint32_t normalBufferHandle = INVALID_HANDLE;
    uint32_t uvBufferHandle = INVALID_HANDLE;
    uint32_t colorBufferHandle = INVALID_HANDLE;

    //Interleaved buffer
    uint32_t vertexBufferHandle = INVALID_HANDLE;

    std::vector<float> indices{};
    std::vector<float> positions{};
    std::vector<float> uvs{};
    std::vector<float> normals{};
    std::vector<float> colors{};

    bool finalized = false;

    bool hasPositions = false;
    bool hasUVs = false;
    bool hasNormals = false;
    bool hasColors = false;
    
    int32_t positionElements = 3;
    constexpr static int32_t NORMAL_ELEMENTS = 3;
    constexpr static int32_t UV_ELEMENTS = 2;
    constexpr static int32_t COLOR_ELEMENTS = 4;
    
    int32_t positionStride = positionElements * sizeof(float);
    constexpr static int32_t NORMAL_STRIDE = NORMAL_ELEMENTS * sizeof(float);
    constexpr static int32_t UV_STRIDE = UV_ELEMENTS * sizeof(float);
    constexpr static int32_t COLOR_STRIDE = COLOR_ELEMENTS * sizeof(float);
  };
}
