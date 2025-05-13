#include "glrender/glrMesh.hh"

#include <glad/gl.hh>

namespace glr
{
  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &this->vertexArrayHandle);
    glDeleteBuffers(1, &this->indexBufferHandle);
    glDeleteBuffers(1, &this->positionBufferHandle);
    glDeleteBuffers(1, &this->normalBufferHandle);
    glDeleteBuffers(1, &this->uvBufferHandle);
    glDeleteBuffers(1, &this->colorBufferHandle);
  }

  Mesh::Mesh(Mesh&& moveFrom) noexcept
  {
    this->bufferType = moveFrom.bufferType;
    this->drawMode = moveFrom.drawMode;
    this->drawType = moveFrom.drawType;

    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;

    this->numIndices = moveFrom.numIndices;
    moveFrom.numIndices = 0;

    this->positionBindingPoint = moveFrom.positionBindingPoint;
    this->normalBindingPoint = moveFrom.normalBindingPoint;
    this->uvBindingPoint = moveFrom.uvBindingPoint;
    this->colorBindingPoint = moveFrom.colorBindingPoint;
    
    this->retainBufferData = moveFrom.retainBufferData;
    moveFrom.retainBufferData = false;


    this->vertexArrayHandle = moveFrom.vertexArrayHandle;
    moveFrom.vertexArrayHandle = INVALID_HANDLE;

    this->positionBufferHandle = moveFrom.positionBufferHandle;
    moveFrom.positionBufferHandle = INVALID_HANDLE;

    this->indexBufferHandle = moveFrom.indexBufferHandle;
    moveFrom.indexBufferHandle = INVALID_HANDLE;

    this->normalBufferHandle = moveFrom.normalBufferHandle;
    moveFrom.normalBufferHandle = INVALID_HANDLE;

    this->uvBufferHandle = moveFrom.uvBufferHandle;
    moveFrom.uvBufferHandle = INVALID_HANDLE;

    this->colorBufferHandle = moveFrom.colorBufferHandle;
    moveFrom.colorBufferHandle = INVALID_HANDLE;


    this->indices = moveFrom.indices;
    moveFrom.indices.clear();

    this->positions = moveFrom.positions;
    moveFrom.positions.clear();

    this->normals = moveFrom.normals;
    moveFrom.normals.clear();

    this->uvs = moveFrom.uvs;
    moveFrom.uvs.clear();
    
    this->colors = moveFrom.colors;
    moveFrom.colors.clear();


    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->hasIndices = moveFrom.hasIndices;
    moveFrom.hasIndices = false;
    
    this->hasPositions = moveFrom.hasPositions;
    moveFrom.hasPositions = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;

    this->hasColors = moveFrom.hasColors;
    moveFrom.hasColors = false;


    this->positionElements = moveFrom.positionElements;
    moveFrom.positionElements = 3;

    this->positionStride = moveFrom.positionStride;
    moveFrom.positionStride = moveFrom.positionElements * sizeof(float);
  }
  
  Mesh& Mesh::operator=(Mesh&& moveFrom) noexcept
  {
    if(this == &moveFrom)
    {
      return *this;
    }
    
    this->bufferType = moveFrom.bufferType;
    this->drawMode = moveFrom.drawMode;
    this->drawType = moveFrom.drawType;

    this->numVerts = moveFrom.numVerts;
    moveFrom.numVerts = 0;

    this->numIndices = moveFrom.numIndices;
    moveFrom.numIndices = 0;

    this->positionBindingPoint = moveFrom.positionBindingPoint;
    this->normalBindingPoint = moveFrom.normalBindingPoint;
    this->uvBindingPoint = moveFrom.uvBindingPoint;
    this->colorBindingPoint = moveFrom.colorBindingPoint;
    
    this->retainBufferData = moveFrom.retainBufferData;
    moveFrom.retainBufferData = false;


    this->vertexArrayHandle = moveFrom.vertexArrayHandle;
    moveFrom.vertexArrayHandle = INVALID_HANDLE;

    this->positionBufferHandle = moveFrom.positionBufferHandle;
    moveFrom.positionBufferHandle = INVALID_HANDLE;

    this->indexBufferHandle = moveFrom.indexBufferHandle;
    moveFrom.indexBufferHandle = INVALID_HANDLE;

    this->normalBufferHandle = moveFrom.normalBufferHandle;
    moveFrom.normalBufferHandle = INVALID_HANDLE;

    this->uvBufferHandle = moveFrom.uvBufferHandle;
    moveFrom.uvBufferHandle = INVALID_HANDLE;

    this->colorBufferHandle = moveFrom.colorBufferHandle;
    moveFrom.colorBufferHandle = INVALID_HANDLE;


    this->indices = moveFrom.indices;
    moveFrom.indices.clear();

    this->positions = moveFrom.positions;
    moveFrom.positions.clear();

    this->normals = moveFrom.normals;
    moveFrom.normals.clear();

    this->uvs = moveFrom.uvs;
    moveFrom.uvs.clear();
    
    this->colors = moveFrom.colors;
    moveFrom.colors.clear();


    this->finalized = moveFrom.finalized;
    moveFrom.finalized = false;
    
    this->hasIndices = moveFrom.hasIndices;
    moveFrom.hasIndices = false;
    
    this->hasPositions = moveFrom.hasPositions;
    moveFrom.hasPositions = false;
    
    this->hasUVs = moveFrom.hasUVs;
    moveFrom.hasUVs = false;
    
    this->hasNormals = moveFrom.hasNormals;
    moveFrom.hasNormals = false;

    this->hasColors = moveFrom.hasColors;
    moveFrom.hasColors = false;


    this->positionElements = moveFrom.positionElements;
    moveFrom.positionElements = 3;

    this->positionStride = moveFrom.positionStride;
    moveFrom.positionStride = moveFrom.positionElements * sizeof(float);
    
    return *this;
  }

  void Mesh::setPositionDimensions(const GLRDimensions dimensions)
  {
    switch(dimensions)
    {
      case GLRDimensions::TWO_DIMENSIONAL:
      {
        this->positionElements = 2;
        break;
      }
      case GLRDimensions::THREE_DIMENSIONAL:
      {
        this->positionElements = 3;
        break;
      }
    }
    this->positionStride = this->positionElements * sizeof(float);
  }

  Mesh* Mesh::addIndices(const uint32_t* indices, const size_t indicesSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(GLRLogType::WARNING, "Mesh::addIndices(): Attempting to add indices to a finalized Mesh\n");
      }
      return this;
    }
    
    this->hasIndices = true;
    this->numIndices = indicesSize;
    this->indices.insert(this->indices.end(), indices, indices + indicesSize);
    return this;
  }

  Mesh* Mesh::addPositions(const float* positions, const size_t positionsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(GLRLogType::WARNING, "Mesh::addPositions(): Attempting to add positions to a finalized Mesh\n");
      }
      return this;
    }
    
    this->hasPositions = true;
    this->numVerts = positionsSize / this->positionElements;
    this->positions.insert(this->positions.end(), positions, positions + positionsSize);
    
    return this;
  }

  Mesh* Mesh::addUVs(const float* uvs, const size_t uvsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(GLRLogType::WARNING, "Mesh::addUVs(): Attempting to add UVs to a finalized Mesh\n");
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
        callback(GLRLogType::WARNING, "Mesh::addNormals(): Attempting to add normals to a finalized Mesh\n");
      }
      return this;
    }
    this->hasNormals = true;
    this->normals.insert(this->normals.end(), normals, normals + normalsSize);
    return this;
  }

  Mesh* Mesh::addColors(const float* colors, const size_t colorsSize, const LoggingCallback& callback)
  {
    if(this->finalized)
    {
      if(callback)
      {
        callback(GLRLogType::WARNING, "Mesh::addColors(): Attempting to add colors to a finalized Mesh\n");
      }
      return this;
    }
    this->hasColors = true;
    this->colors.insert(this->colors.end(), colors, colors + colorsSize);
    return this;
  }
  
  void Mesh::finalize(const LoggingCallback& callback)
  {
    //Sanity checks
    if(!this->hasPositions || this->positions.empty())
    {
      if(callback)
      {
        callback(GLRLogType::ERROR, "Mesh::finalize(): Can't finalize Mesh, no verticies have been added\n");
      }
      return;
    }

    if(this->finalized)
    {
      if(callback)
      {
        callback(GLRLogType::ERROR, "Mesh::finalize(): Can't finalize Mesh, Mesh has already been finalized\n");
      }
      return;
    }

    if(callback)
    {
      if(!this->positions.empty() && this->positions.size() % this->positionElements != 0)
      {
        const std::string elements = std::to_string(this->positionElements);
        callback(GLRLogType::WARNING, "Mesh::finalize(): The number of position elements that have been added is not divisible by " + elements + ", this will cause unintended effects\n");
      }
      if(this->hasNormals && !this->normals.empty() && this->normals.size() % NORMAL_ELEMENTS != 0)
      {
        callback(GLRLogType::WARNING, "Mesh::finalize(): The number of normal elements that have been added is not divisible by 3, this will cause unintended effects\n");
      }
      if(this->hasUVs && !this->uvs.empty() && this->uvs.size() % UV_ELEMENTS != 0)
      {
        callback(GLRLogType::WARNING, "Mesh::finalize(): The number of UV elements that have been added is not divisible by 2, this will cause unintended effects\n");
      }
      if(this->hasColors && !this->colors.empty() && this->colors.size() % COLOR_ELEMENTS != 0)
      {
        callback(GLRLogType::WARNING, "Mesh::finalize(): The number of color elements that have been added is not divisible by 4, this will cause unintended effects\n");
      }
    }
    
    //Create a vertex array to hold all of our state for this mesh, reducing API call overhead when switching meshes
    glCreateVertexArrays(1, &this->vertexArrayHandle);
    
    //TODO FIXME interleaved buffer segfaults
    /*if(this->bufferType == GLBufferType::INTERLEAVED)
    {
      //Create a buffer in the GPU's VRAM to hold our interleaved vertex data
      glCreateBuffers(1, &this->vertexBufferHandle);
      
      //Calculate the total elements in the interleaved buffer
      size_t total = this->positions.size();
      if(this->hasNormals)
      {
        total += this->normals.size();
      }
      if(this->hasUVs)
      {
        total += this->uvs.size();
      }
      if(this->hasColors)
      {
        total += this->colors.size();
      }
    
      std::vector<float> buffer{};
      buffer.reserve(total);

      //Interleave our vertex data: position-uv-normal-color
      for(int64_t i = 0; i < (int64_t)this->numVerts; i++)
      {
        buffer.insert(buffer.end(), this->positions.begin() + i * this->positionElements, this->positions.begin() + i * this->positionElements + this->positionElements);
        if(this->hasUVs)
        {
          buffer.insert(buffer.end(), this->uvs.begin() + i * UV_ELEMENTS, this->uvs.begin() + i * UV_ELEMENTS + UV_ELEMENTS);
        }
        if(this->hasNormals)
        {
          buffer.insert(buffer.end(), this->normals.begin() + i * NORMAL_ELEMENTS, this->normals.begin() + i * NORMAL_ELEMENTS + NORMAL_ELEMENTS);
        }
        if(this->hasColors)
        {
          buffer.insert(buffer.end(), this->colors.begin() + i * COLOR_ELEMENTS, this->colors.begin() + i * COLOR_ELEMENTS + COLOR_ELEMENTS);
        }
      }
      
      //Calculate the stride between the start of one vertex and the start of the next
      int32_t stride = this->positionStride;
      if(this->hasNormals)
      {
        stride += NORMAL_STRIDE;
      }
      if(this->hasUVs)
      {
        stride += UV_STRIDE;
      }
      if(this->hasColors)
      {
        stride += COLOR_STRIDE;
      }
      
      //Upload our vertex data to the buffer we created
      glNamedBufferData(this->vertexBufferHandle, (GLsizeiptr)(buffer.size() * sizeof(float)), buffer.data(), this->getGLDrawType());
    
      //Bind a buffer to our vertex array and give it the stride information
      glVertexArrayVertexBuffer(this->vertexArrayHandle, positionBindingPoint, this->vertexBufferHandle, 0, stride);
    
      //Set up attributes for our potentially interleaved data buffer so OpenGL knows how to read data out of it
      glEnableVertexArrayAttrib(this->vertexArrayHandle, this->positionBindingPoint);
      glVertexArrayAttribBinding(this->vertexArrayHandle, this->positionBindingPoint, this->vertexBufferHandle);
      glVertexArrayAttribFormat(this->vertexArrayHandle, this->positionBindingPoint, 3, GL_FLOAT, GL_FALSE, 0);
    
      if(this->hasNormals)
      {
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->normalBindingPoint);
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->normalBindingPoint, this->vertexBufferHandle);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->normalBindingPoint, 3, GL_FLOAT, GL_FALSE, this->positionStride);
      }
      if(this->hasUVs)
      {
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->uvBindingPoint);
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->uvBindingPoint, this->vertexBufferHandle);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->uvBindingPoint, 2, GL_FLOAT, GL_FALSE, this->positionStride + NORMAL_STRIDE);
      }
      if(this->hasColors)
      {
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->colorBindingPoint);
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->colorBindingPoint, this->vertexBufferHandle);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->colorBindingPoint, 4, GL_FLOAT, GL_FALSE, this->positionStride + NORMAL_STRIDE + UV_STRIDE);
      }
    }
    else*/ if(this->bufferType == GLRBufferType::SEPARATE)
    {
      //Create a buffer in the GPU's VRAM to hold our vertex position data
      glCreateBuffers(1, &this->positionBufferHandle);
      
      //Upload our position data to the buffer we created
      glNamedBufferData(this->positionBufferHandle, (GLsizeiptr)(this->positions.size() * sizeof(float)), this->positions.data(), this->getGLDrawType());

      //Bind the attribute to an index in the shaders
      glVertexArrayAttribBinding(this->vertexArrayHandle, this->positionBindingPoint, this->positionBindingPoint);
      
      //Bind the buffer to our vertex array and give it the stride information
      glVertexArrayVertexBuffer(this->vertexArrayHandle, this->positionBindingPoint, this->positionBufferHandle, 0, this->positionStride);

      //Enable the attribute for our data buffer
      glEnableVertexArrayAttrib(this->vertexArrayHandle, this->positionBindingPoint);

      //Define the format of the buffer so OpenGL knows how to read data out of it
      glVertexArrayAttribFormat(this->vertexArrayHandle, this->positionBindingPoint, this->positionElements, GL_FLOAT, GL_FALSE, 0);
      
      //Repeat for any other vertex data
      if(this->hasIndices)
      {
        glCreateBuffers(1, &this->indexBufferHandle);
        glNamedBufferData(this->indexBufferHandle, (GLsizeiptr)(this->indices.size() * sizeof(uint32_t)), this->indices.data(), this->getGLDrawType());
        glVertexArrayElementBuffer(this->vertexArrayHandle, this->indexBufferHandle);
      }
      if(this->hasNormals)
      {
        glCreateBuffers(1, &this->normalBufferHandle);
        glNamedBufferData(this->normalBufferHandle, (GLsizeiptr)(this->normals.size() * sizeof(float)), this->normals.data(), this->getGLDrawType());
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->normalBindingPoint, this->normalBindingPoint);
        glVertexArrayVertexBuffer(this->vertexArrayHandle, this->normalBindingPoint, this->normalBufferHandle, 0, NORMAL_STRIDE);
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->normalBindingPoint);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->normalBindingPoint, NORMAL_ELEMENTS, GL_FLOAT, GL_FALSE, 0);
      }
      if(this->hasUVs)
      {
        glCreateBuffers(1, &this->uvBufferHandle);
        glNamedBufferData(this->uvBufferHandle, (GLsizeiptr)(this->uvs.size() * sizeof(float)), this->uvs.data(), this->getGLDrawType());
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->uvBindingPoint, this->uvBindingPoint);
        glVertexArrayVertexBuffer(this->vertexArrayHandle, this->uvBindingPoint, this->uvBufferHandle, 0, UV_STRIDE);
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->uvBindingPoint);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->uvBindingPoint, UV_ELEMENTS, GL_FLOAT, GL_FALSE, 0);
      }
      if(this->hasColors)
      {
        glCreateBuffers(1, &this->colorBufferHandle);
        glNamedBufferData(this->colorBufferHandle, (GLsizeiptr)(this->colors.size() * sizeof(float)), this->colors.data(), this->getGLDrawType());
        glVertexArrayAttribBinding(this->vertexArrayHandle, this->colorBindingPoint, this->colorBindingPoint);
        glVertexArrayVertexBuffer(this->vertexArrayHandle, this->colorBindingPoint, this->colorBufferHandle, 0, COLOR_STRIDE);
        glEnableVertexArrayAttrib(this->vertexArrayHandle, this->colorBindingPoint);
        glVertexArrayAttribFormat(this->vertexArrayHandle, this->colorBindingPoint, COLOR_ELEMENTS, GL_FLOAT, GL_FALSE, 0);
      }
    }
    if(!this->retainBufferData)
    {
      this->indices.clear();
      this->positions.clear();
      this->normals.clear();
      this->uvs.clear();
      this->colors.clear();
    }
    this->finalized = true;
  }
  
  void Mesh::use() const
  {
    if(this->finalized)
    {
      glBindVertexArray(this->vertexArrayHandle);
    }
  }

  bool Mesh::isFinalized() const
  {
    return this->finalized;
  }

  bool Mesh::isIndexed() const
  {
    return this->hasIndices;
  }

  int Mesh::getGLDrawType() const
  {
    switch(this->drawType)
    {
      case GLRDrawType::STATIC:
      {
        return GL_STATIC_DRAW;
      }
      case GLRDrawType::STREAM:
      {
        return GL_STREAM_DRAW;
      }
      case GLRDrawType::DYNAMIC:
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
