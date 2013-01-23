#pragma once

class InstancedTriangleSet;

class Mesh {
public:
  Mesh(InstancedTriangleSet * shape);
  ~Mesh();

  void inflate();
  void deflate();
  void generateNormals();
  void subdivide(int lod);
  void normalize();

private:
  InstancedTriangleSet * self;
};