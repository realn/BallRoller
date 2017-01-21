#pragma once

#include "MeshBuffer.h"

class CMeshCubeBuilderBase {
protected:
  enum class Side {
    Front = 0,
    Back = 1,
    Left = 2,
    Right = 3,
    Top = 4,
    Bottom = 5
  };

public:
  CMeshCubeBuilderBase();
  virtual ~CMeshCubeBuilderBase();

  void BuildCube(const glm::float32 size, const glm::vec3 offset = glm::vec3(0.0f));
  
private:
  void BuildSide(const glm::float32 size, const Side side, const glm::vec3& offset);
  void BuildVertex(const glm::float32 x, 
                   const glm::float32 y, 
                   const glm::float32 z, 
                   const Side size, 
                   const glm::vec3& offset);

protected:
  virtual void AddVertex(const glm::vec3& pos, 
                         const glm::vec3& pass,
                         const glm::vec3& offset, 
                         const glm::vec3& sideNormal, 
                         const Side side) = 0;
  virtual void BeginBuild() = 0;
  virtual void EndBuild() = 0;
};

template<typename _Vertex>
class CMeshCubeBuilder :
  public CMeshCubeBuilderBase {
private:
  CMeshBuffer<_Vertex>* mpMesh;
  std::vector<_Vertex> mVertBuffer;

public:
  CMeshCubeBuilder() : mpMesh(nullptr) {}
  virtual ~CMeshCubeBuilder() { delete mpMesh; }

  CMeshBuffer<_Vertex>* GetMesh() const {
    return mpMesh;
  }

protected:
  const _Vertex MakeVertex(const glm::vec3 & pos, 
                           const glm::vec3 & pass, 
                           const glm::vec3 & offset, 
                           const glm::vec3 & sideNormal, 
                           const Side side) {
    return _Vertex(pos);
  }
  void AddQuad() {
    mpMesh->AddQuadCCW(mVertBuffer[0],
                       mVertBuffer[1],
                       mVertBuffer[2],
                       mVertBuffer[3]);
  }

  // Inherited via CMeshCubeBuilderBase
  virtual void AddVertex(const glm::vec3 & pos, 
                         const glm::vec3 & pass, 
                         const glm::vec3 & offset, 
                         const glm::vec3 & sideNormal, 
                         const Side side) override 
  {
    {
      _Vertex v = MakeVertex(pos, pass, offset, sideNormal, side);
      mVertBuffer.push_back(v);
    }
    if(mVertBuffer.size() == 4) {
      AddQuad();
      mVertBuffer.clear();
    }
  }

  virtual void BeginBuild() override {
    delete mpMesh;
    mpMesh = new CMeshBuffer<_Vertex>();
  }

  virtual void EndBuild() override {}
};

class CMeshSphereBuilderBase {
public:
  CMeshSphereBuilderBase();
  ~CMeshSphereBuilderBase();
};