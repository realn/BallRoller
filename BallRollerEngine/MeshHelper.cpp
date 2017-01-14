#include "stdafx.h"
#include "MeshHelper.h"

CMeshCubeBuilderBase::CMeshCubeBuilderBase() {}

CMeshCubeBuilderBase::~CMeshCubeBuilderBase() {}

void CMeshCubeBuilderBase::BuildCube(const float size, const glm::vec3 offset) {
  BeginBuild();
  for(int i = 0; i < 6; i++) {
    BuildSide(size, (Side)i, offset);
  }
  EndBuild();
}

void CMeshCubeBuilderBase::BuildSide(const float size, const Side side, const glm::vec3 & offset) {
  float half = size / 2.0f;
  BuildVertex(-half, -half, half, side, offset);
  BuildVertex(half, -half, half, side, offset);
  BuildVertex(half, half, half, side, offset);
  BuildVertex(-half, half, half, side, offset);
}

void CMeshCubeBuilderBase::BuildVertex(const float x, const float y, const float z, const Side side, const glm::vec3 & offset) {
  glm::vec3 pass(x, y, z);
  switch(side) {
  case Side::Front: 
    AddVertex(glm::vec3(x, y, z), pass, offset, glm::vec3(0.0f, 0.0f, 1.0f), side); break;
  case Side::Back: 
    AddVertex(glm::vec3(-x, y, -z), pass, offset, glm::vec3(0.0f, 0.0f, -1.0f), side); break;
  case Side::Left: 
    AddVertex(glm::vec3(z, y, -x), pass, offset, glm::vec3(1.0f, 0.0f, 0.0f), side); break;
  case Side::Right: 
    AddVertex(glm::vec3(-z, y, x), pass, offset, glm::vec3(-1.0f, 0.0f, 0.0f), side); break;
  case Side::Top: 
    AddVertex(glm::vec3(-x, z, y), pass, offset, glm::vec3(0.0f, 1.0f, 0.0f), side); break;
  case Side::Bottom: 
    AddVertex(glm::vec3(x, z, -y), pass, offset, glm::vec3(0.0f,-1.0f, 0.0f), side); break;
  default:
    break;
  }
}

CMeshSphereBuilderBase::CMeshSphereBuilderBase() {}

CMeshSphereBuilderBase::~CMeshSphereBuilderBase() {}
