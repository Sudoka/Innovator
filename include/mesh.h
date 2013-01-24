#pragma once

#include <vector>
#include <glm/glm.hpp>

class InstancedTriangleSet;

class Mesh {
public:
  Mesh(InstancedTriangleSet * shape);
  ~Mesh();

  void inflate();
  void deflate();
  void subdivide();

  void generatePerFaceNormals();
  void generatePerVertexNormals();

private:
  glm::vec3 getFaceNormal(const glm::ivec3 & face);

private:
  InstancedTriangleSet * self;
};