#pragma once

#include "GLutils.h"
#include <vector>

class CVertexDefinition {
private:
  struct CStream {
    glm::int32 mAttribute;
    glm::uint32 mSize;
    glm::uint32 mType;
    glm::uint32 mOffset;
    bool mNormalized;
  };
  typedef std::vector<CStream> streamvec;

  streamvec mStreams;
  glm::uint32 mStride;

public:
  CVertexDefinition(const glm::uint32 stride);
  CVertexDefinition(const CVertexDefinition& other);
  ~CVertexDefinition();

  void AddStream(const glm::int32 attribute, 
                 const glm::uint32 size, 
                 const glm::uint32 type, 
                 const glm::uint32 offset, 
                 const bool normalized = false);
  void Clear();

  void Bind() const;
  void Unbind() const;

  void operator=(const CVertexDefinition& other);
};