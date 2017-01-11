#pragma once

#include <GLES2/gl2.h>
#include <vector>

class CVertexDefinition {
private:
  struct CStream {
    GLint mAttribute;
    GLuint mSize;
    GLenum mType;
    GLsizei mOffset;
    bool mNormalized;
  };
  typedef std::vector<CStream> streamvec;

  streamvec mStreams;
  GLsizei mStride;

public:
  CVertexDefinition(const GLsizei stride);
  CVertexDefinition(const CVertexDefinition& other);
  ~CVertexDefinition();

  void AddStream(const GLint attribute, const GLuint size, const GLenum type, const GLsizei offset, const bool normalized = false);
  void Clear();

  void Bind() const;
  void Unbind() const;

  void operator=(const CVertexDefinition& other);
};