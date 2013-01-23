#pragma once

#include <memory>

class InstancedTriangleSet;

class Mesh {
public:
  Mesh(InstancedTriangleSet * shape);
  ~Mesh();

  void inflate();
  void deflate();
  void generateNormals();
  void subdivide(int lod);

private:
  class MeshP;
  std::unique_ptr<MeshP> self;
};