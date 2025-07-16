#pragma once

enum struct GLRLogType
{
  INFO, WARNING, ERROR, DBG
};

enum struct GLRIOMode : unsigned short
{
  READ = 0x88B8, WRITE = 0x88B9, READ_WRITE = 0x88BA
};

enum struct GLRFilterMode
{
  NEAREST,
  BILINEAR,
  TRILINEAR
};

enum class GLRDrawMode : unsigned short
{
  TRIS = 0x0004,
  TRI_STRIPS = 0x0005,
  TRI_FANS = 0x0006,
  LINES = 0x0001,
  LINE_STRIPS = 0x0003,
  LINE_LOOPS = 0x0002,
  POINTS = 0x0000,
  NONE,
};

enum class GLRColorFormat : unsigned short
{
  R32F = 0x822E,
  RGB8 = 0x8051,
  RGBA8 = 0x8058,
  RGB16 = 0x8054,
  RGBA16 = 0x805B,
  RGB32I = 0x8D83,
  RGBA32I = 0x8D82,
  RGB32UI = 0x8D71,
  RGBA32UI = 0x8D70,
  RGB16F = 0x881B,
  RGBA16F = 0x881A,
  RGB32F = 0x8815,
  RGBA32F = 0x8814,
  DEPTH32F = 0x8CAC,
};

enum class GLRDrawType : unsigned short
{
  STREAM_COPY = 0x88E2, STREAM_DRAW = 0x88E0, STREAM_READ = 0x88E1,
  STATIC_COPY = 0x88E6, STATIC_DRAW = 0x88E4, STATIC_READ = 0x88E5,
  DYNAMIC_COPY = 0x88EA, DYNAMIC_DRAW = 0x88E8, DYNAMIC_READ = 0x88E9,
  NONE,
};

enum struct GLRBufferType
{
  INTERLEAVED, SEPARATE, NONE,
};

enum struct GLRDimensions
{
  TWO_DIMENSIONAL, THREE_DIMENSIONAL,
};

enum struct GLRAttachment
{
  COLOR, DEPTH, STENCIL,
};

enum struct GLRAttachmentType
{
  TEXTURE, RENDER_BUFFER
};

enum struct GLRShaderType
{
  INVALID, FRAG_VERT, COMPUTE, GEOMETRY_FRAG, TESSELLATION,
};

enum struct GLRIndexBufferType : unsigned short
{
  UINT = 0x1405, INT = 0x1404,
};

enum struct GLRBlendMode : unsigned short
{
  ZERO = 0, ONE = 1, SRC_COLOR = 0x0300, ONE_MINUS_SRC_COLOR = 0x0301, DST_COLOR = 0x0306, ONE_MINUS_DST_COLOR = 0x0307,
  SRC_ALPHA = 0x0302, ONE_MINUS_SRC_ALPHA = 0x0303, DST_ALPHA = 0x0304, ONE_MINUS_DST_ALPHA = 0x0305,
  CONSTANT_COLOR = 0x8001, ONE_MINUS_CONSTANT_COLOR = 0x8002, CONSTANT_ALPHA = 0x8003, ONE_MINUS_CONSTANT_ALPHA = 0x8004,
};

enum struct GLRClearType : unsigned int
{
  NONE = 0,
  COLOR = 0x00004000, DEPTH = 0x00000100, STENCIL = 0x00000400,
};
