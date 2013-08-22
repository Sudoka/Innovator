#pragma once

#include <vector>
#include <memory>
#include <opengl.h>

class GLMatrix;
class GLDrawCall;
class GLVertexArrayObject;

class DrawCache {
public:
  DrawCache(GLProgram * program,
            GLMaterial * material,
            GLMatrix * transform,
            GLVertexArrayObject * vao,
            GLDrawCall * drawcall);

  void flush();

  GLProgram * program;
  GLMatrix * transform;
  GLDrawCall * drawcall;
  GLVertexArrayObject * vao;
  GLMaterial * material;
};

class RenderCache {
public:
  RenderCache();
  ~RenderCache();

  void compile();
  void flush();

  std::vector<DrawCache> drawlist;
  //std::vector<std::function<void()>> glcalls;
};