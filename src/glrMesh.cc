#include "GLRender/glrMesh.hh"

#include <glad/gl.hh>

namespace glr
{
  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &this->vertexArrayHandle);
    glDeleteBuffers(1, &this->vertexBufferHandle);
  }

  Mesh::Mesh(Mesh&& moveFrom) noexcept
  {
    this->vertexBufferHandle = moveFrom.vertexBufferHandle;
    moveFrom.vertexBufferHandle = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
    this->hasPositions = moveFrom.hasPositions;
    moveFrom.hasPositions = false;
    
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

    this->vertexBufferHandle = moveFrom.vertexBufferHandle;
    moveFrom.vertexBufferHandle = INVALID_HANDLE;
    
    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;
    
    this->hasPositions = moveFrom.hasPositions;
    moveFrom.hasPositions = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;
    
    return *this;
  }

  Mesh* Mesh::addPositions(const float* positions, const size_t positionsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(LogType::ERROR, "Mesh::addVerts(): Attempting to add verticies to a finalized Mesh\n");
      }
      return this;
    }
    
    this->hasPositions = true;
    this->numVerts = positionsSize / 3;
    this->positions.insert(this->positions.end(), positions, positions + positionsSize);
    
    return this;
  }

  Mesh* Mesh::addUVs(const float* uvs, const size_t uvsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(LogType::ERROR, "Mesh::addUVs(): Attempting to add UVs to a finalized Mesh\n");
      }
      return this;
    }
    
    this->hasUVs = true;
    this->uvs.insert(this->uvs.end(), uvs, uvs + uvsSize);
    
    return this;
  }

  Mesh* Mesh::addNormals(const float* normals, const size_t normalsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(LogType::ERROR, "Mesh::addNormals(): Attempting to add normals to a finalized Mesh\n");
      }
      return this;
    }
    
    this->hasNormals = true;
    this->normals.insert(this->normals.end(), normals, normals + normalsSize);
    
    return this;
  }

  void Mesh::finalize(const LoggingCallback& callback)
  {
    if(!this->hasPositions || this->positions.empty())
    {
      if(callback)
      {
        callback(LogType::ERROR, "Mesh::finalize(): Can't finalize Mesh, no verticies have been added\n");
      }
      return;
    }

    if(this->finalized)
    {
      if(callback)
      {
        callback(LogType::WARNING, "Mesh::finalize(): Can't finalize Mesh, Mesh has already been finalized\n");
      }
      return;
    }

    if(this->vertexArrayHandle == INVALID_HANDLE)
    {
      //VAOs hold all the state for a VBO, reducing API call overhead
      glCreateVertexArrays(1, &this->vertexArrayHandle);
    }

    //Interleave data: verts normals uvs
    const size_t total = this->positions.size() + this->hasNormals ? this->normals.size() : 0 + this->hasUVs ? this->uvs.size() : 0;
    std::vector<float> buffer{};
    buffer.reserve(total);
    for(size_t i = 0; i < this->positions.size(); i++)
    {
      buffer.insert(buffer.end(), this->positions.begin() + i * 3, this->positions.begin() + i * 3 + 3);
      if(this->hasNormals)
      {
        buffer.insert(buffer.end(), this->normals.begin() + i * 3, this->normals.begin() + i * 3 + 3);
      }
      if(this->hasUVs)
      {
        buffer.insert(buffer.end(), this->uvs.begin() + i * 2, this->uvs.begin() + i * 2 + 2);
      }
    }

    //Create a buffer to hold our interleaved vertex data
    glCreateBuffers(1, &this->vertexBufferHandle);

    //Upload our buffer to the GPU's VRAM
    glNamedBufferData(this->vertexBufferHandle, (GLsizeiptr)(buffer.size() * sizeof(float)), buffer.data(), this->getGLDrawType());

    //Calculate the stride between the start of one vertex and the start of the next
    int32_t stride = Mesh::POSITION_STRIDE;
    if(this->hasNormals)
    {
      stride += Mesh::NORMAL_STRIDE;
    }
    if(this->hasUVs)
    {
      stride += Mesh::UV_STRIDE;
    }
    
    //Bind a buffer to our vertex array and give it the stride information
    glVertexArrayVertexBuffer(this->vertexArrayHandle, 0, this->vertexBufferHandle, 0, stride);
    
    //Set up attributes for our interleaved data buffer so OpenGL knows how to read data out of it
    glEnableVertexArrayAttrib(this->vertexArrayHandle, Mesh::POSITION_BINDING_POINT);
    glVertexArrayAttribBinding(this->vertexArrayHandle, Mesh::POSITION_BINDING_POINT, Mesh::POSITION_BINDING_POINT);
    glVertexArrayAttribFormat(this->vertexArrayHandle, Mesh::POSITION_BINDING_POINT, 3, GL_FLOAT, GL_FALSE, 0);
    
    if(this->hasNormals)
    {
      glEnableVertexArrayAttrib(this->vertexArrayHandle, Mesh::NORMAL_BINDING_POINT);
      glVertexArrayAttribBinding(this->vertexArrayHandle, Mesh::NORMAL_BINDING_POINT, Mesh::NORMAL_BINDING_POINT);
      glVertexArrayAttribFormat(this->vertexArrayHandle, Mesh::NORMAL_BINDING_POINT, 3, GL_FLOAT, GL_FALSE, this->POSITION_STRIDE);
    }
    
    if(this->hasUVs)
    {
      glEnableVertexArrayAttrib(this->vertexArrayHandle, Mesh::UV_BINDING_POINT);
      glVertexArrayAttribBinding(this->vertexArrayHandle, Mesh::UV_BINDING_POINT, Mesh::UV_BINDING_POINT);
      glVertexArrayAttribFormat(this->vertexArrayHandle, Mesh::UV_BINDING_POINT, 2, GL_FLOAT, GL_FALSE, this->POSITION_STRIDE + this->NORMAL_STRIDE);
    }
    
    this->finalized = true;
  }
  
  void Mesh::use() const
  {
    if(this->finalized)
    {
      glBindVertexArray(this->vertexBufferHandle);
    }
  }

  bool Mesh::isFinalized() const
  {
    return this->finalized;
  }

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
}
