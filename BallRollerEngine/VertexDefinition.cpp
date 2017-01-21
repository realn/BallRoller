#include "stdafx.h"
#include "VertexDefinition.h"

CVertexDefinition::CVertexDefinition(const glm::uint32 stride) 
  : mStride(stride) {}

CVertexDefinition::CVertexDefinition(const CVertexDefinition & other) 
  : mStreams(other.mStreams), mStride(other.mStride) {}

CVertexDefinition::~CVertexDefinition() {}

void CVertexDefinition::AddStream(const glm::int32 attribute, 
                                  const glm::uint32 size, 
                                  const glm::uint32 type, 
                                  const glm::uint32 offset, 
                                  const bool normalized) 
{
  CStream s;
  s.mAttribute = attribute;
  s.mSize = size;
  s.mType = type;
  s.mOffset = offset;
  s.mNormalized = normalized;
  mStreams.push_back(s);
}

void CVertexDefinition::Clear() {
  mStreams.clear();
}

void CVertexDefinition::Bind() const {
  for(streamvec::const_iterator it = mStreams.begin(); it != mStreams.end(); it++) {
    glVertexAttribPointer(it->mAttribute,
                          it->mSize,
                          it->mType,
                          it->mNormalized ? GL_TRUE : GL_FALSE,
                          mStride,
                          reinterpret_cast<const void*>(it->mOffset));

    glEnableVertexAttribArray(it->mAttribute);
  }
}

void CVertexDefinition::Unbind() const {
  for(streamvec::const_iterator it = mStreams.begin(); it != mStreams.end(); it++) {
    glDisableVertexAttribArray(it->mAttribute);
  }
}

void CVertexDefinition::operator=(const CVertexDefinition & other) {
  mStreams = other.mStreams;
  mStride = other.mStride;
}
