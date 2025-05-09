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
  
  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &this->vertArrayHandle);
    glDeleteBuffers(1, &this->vertBufferHandle);
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
    if(this->hasVerts)
    {
      return this;
    }

    this->hasVerts = true;
    this->numVerts = vertsSize / 3;
    this->verts = verts;
    this->vertsSize = vertsSize;
    
    return this;
  }

  Mesh* Mesh::addUVs(const float* uvs, const size_t uvsSize)
  {
    if(this->hasUVs)
    {
      return this;
    }

    this->hasUVs = true;
    this->uvs = uvs;
    this->uvsSize = uvsSize;
    
    return this;
  }

  Mesh* Mesh::addNormals(const float* normals, const size_t normalsSize)
  {
    if(this->hasNormals)
    {
      return this;
    }
    
    this->hasNormals = true;
    this->normals = normals;
    this->normalsSize = normalsSize;
    
    return this;
  }

  void Mesh::finalize()
  {
    if(!this->hasVerts || this->vertsSize == 0)
    {
      printf("Mesh error: can't finalize Mesh, no verticies have been added\n");
      return;
    }

    if(this->vertArrayHandle == INVALID_HANDLE)
    {
      glCreateVertexArrays(1, &this->vertArrayHandle);
    }

    //Interleave data: verts normals uvs
    const size_t total = this->vertsSize + this->hasNormals ? this->normalsSize : 0 + this->hasUVs ? this->uvsSize : 0;
    std::vector<float> buffer{};
    buffer.reserve(total);
    for(size_t i = 0; i < this->vertsSize; i++)
    {
      buffer.insert(buffer.end(), this->verts + i * 3, this->verts + i * 3 + 3);
      if(this->hasNormals)
      {
        buffer.insert(buffer.end(), this->normals + i * 3, this->normals + i * 3 + 3);
      }
      if(this->hasUVs)
      {
        buffer.insert(buffer.end(), this->uvs + i * 2, this->uvs + i * 2 + 2);
      }
    }

    glCreateBuffers(1, &this->vertBufferHandle);
    glNamedBufferData(this->vertBufferHandle, (GLsizeiptr)(buffer.size() * sizeof(float)), buffer.data(), this->getGLDrawType());
    glVertexArrayAttribBinding(this->vertArrayHandle, 0, 0);
    glVertexArrayVertexBuffer(this->vertArrayHandle, 0, this->vertBufferHandle, 0, this->vertexStride);
    glEnableVertexArrayAttrib(this->vertArrayHandle, 0);
    glVertexArrayAttribFormat(this->vertArrayHandle, 0, 3 + this->hasNormals ? 3 : 0 + this->hasUVs ? 2 : 0, GL_FLOAT, GL_FALSE, 0);
  }
  
  Mesh::Mesh(Mesh&& moveFrom) noexcept
  {
    this->vertBufferHandle = moveFrom.vertBufferHandle;
    moveFrom.vertBufferHandle = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
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

    this->vertBufferHandle = moveFrom.vertBufferHandle;
    moveFrom.vertBufferHandle = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
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
    glBindVertexArray(this->vertBufferHandle);
  }
}
