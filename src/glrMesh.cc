#include "GLRender/glrMesh.hh"

#include <glad/gl.hh>

namespace glr
{
  int Mesh::getGLDrawType() const
  {
    switch(this->type)
    {
      case GLDrawType::STATIC:
      {
        return GL_STATIC_DRAW;
      }
      case GLDrawType::STREAM:
      {
        return GL_STREAM_DRAW;
      }
      case GLDrawType::DYNAMIC:
      {
        return GL_DYNAMIC_DRAW;
      }
      default:
      {
        return GL_STATIC_DRAW;
      }
    }
  }
  
  Mesh::Mesh()
  {
    glCreateVertexArrays(1, &this->vao);
  }
  
  Mesh::~Mesh()
  {
    glDeleteBuffers(1, &this->vboV);
    glDeleteBuffers(1, &this->vboU);
    glDeleteBuffers(1, &this->vboN);
    glDeleteBuffers(1, &this->vboI);
    glDeleteVertexArrays(1, &this->vao);
  }

  void Mesh::setDrawType(const GLDrawType type)
  {
    this->type = type;
  }

  GLDrawType Mesh::getDrawType() const
  {
    return this->type;
  }

  void Mesh::setDrawMode(GLDrawMode mode)
  {
    this->mode = mode;
  }

  GLDrawMode Mesh::getDrawMode() const
  {
    return this->mode;
  }

  Mesh* Mesh::addVerts(const float* verts, const size_t vertsSize)
  {
    if(this->hasIndexedVerts || this->hasVerts)
    {
      return this;
    }
    
    this->hasVerts = true;
    this->numVerts = vertsSize / 3;
  
    glCreateBuffers(1, &this->vboV);
    glNamedBufferData(this->vboV, (GLsizeiptr)(vertsSize * sizeof(float)), verts, this->getGLDrawType());
    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayVertexBuffer(this->vao, 0, this->vboV, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vao, 0);
    glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    return this;
  }

  Mesh* Mesh::addIndexedVerts(const uint32_t* indices, const size_t indicesSize, const float* verts, const size_t vertsSize)
  {
    if(this->hasVerts || this->hasIndexedVerts)
    {
      return this;
    }
    
    this->hasIndexedVerts = true;
    this->numVerts = vertsSize / 3;
  
    glCreateBuffers(1, &this->vboV);
    glNamedBufferData(this->vboV, (GLsizeiptr)(vertsSize * sizeof(float)), verts, this->getGLDrawType());
    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayVertexBuffer(this->vao, 0, this->vboV, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vao, 0);
    glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
  
    glCreateBuffers(1, &this->vboI);
    glNamedBufferData(this->vboI, (GLsizeiptr)(indicesSize * sizeof(uint32_t)), indices, this->getGLDrawType());
    glVertexArrayElementBuffer(this->vao, this->vboI);
    return this;
  }

  Mesh* Mesh::addUVs(const float* uvs, const size_t uvsSize)
  {
    if(this->hasUVs)
    {
      return this;
    }
    
    this->hasUVs = true;
    glCreateBuffers(1, &this->vboU);
    glNamedBufferData(this->vboU, (GLsizeiptr)(uvsSize * sizeof(float)), uvs, this->getGLDrawType());
    glVertexArrayAttribBinding(this->vao, 1, 1);
    glVertexArrayVertexBuffer(this->vao, 1, this->vboU, 0, this->uvStride);
    glEnableVertexArrayAttrib(this->vao, 1);
    glVertexArrayAttribFormat(this->vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
    return this;
  }

  Mesh* Mesh::addNormals(const float* normals, const size_t normalsSize)
  {
    if(this->hasNormals)
    {
      return this;
    }
    
    this->hasNormals = true;
    glCreateBuffers(1, &this->vboN);
    glNamedBufferData(this->vboN, (GLsizeiptr)(normalsSize * sizeof(float)), normals, this->getGLDrawType());
    glVertexArrayAttribBinding(this->vao, 2, 2);
    glVertexArrayVertexBuffer(this->vao, 2, this->vboN, 0, this->normalStride);
    glEnableVertexArrayAttrib(this->vao, 2);
    glVertexArrayAttribFormat(this->vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
    return this;
  }
  
  Mesh::Mesh(Mesh&& moveFrom) noexcept
  {
    this->vboV = moveFrom.vboV;
    moveFrom.vboV = INVALID_HANDLE;
    
    this->vboU = moveFrom.vboU;
    moveFrom.vboU = INVALID_HANDLE;
    
    this->vboN = moveFrom.vboN;
    moveFrom.vboN = INVALID_HANDLE;
    
    this->vboI = moveFrom.vboI;
    moveFrom.vboI = INVALID_HANDLE;
    
    this->vao = moveFrom.vao;
    moveFrom.vao = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;

    this->hasIndexedVerts = moveFrom.hasIndexedVerts;
    moveFrom.hasIndexedVerts = false;
    
    this->hasVerts = moveFrom.hasVerts;
    moveFrom.hasVerts = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;
  }
  
  Mesh& Mesh::operator=(Mesh&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->vboV = moveFrom.vboV;
    moveFrom.vboV = INVALID_HANDLE;
    
    this->vboU = moveFrom.vboU;
    moveFrom.vboU = INVALID_HANDLE;
    
    this->vboN = moveFrom.vboN;
    moveFrom.vboN = INVALID_HANDLE;
    
    this->vboI = moveFrom.vboI;
    moveFrom.vboI = INVALID_HANDLE;
    
    this->vao = moveFrom.vao;
    moveFrom.vao = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;

    this->hasIndexedVerts = moveFrom.hasIndexedVerts;
    moveFrom.hasIndexedVerts = false;
    this->hasVerts = moveFrom.hasVerts;
    moveFrom.hasVerts = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;
    
    return *this;
  }
  
  void Mesh::use() const
  {
    glBindVertexArray(this->vao);
  }
}
