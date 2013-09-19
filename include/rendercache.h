#pragma once

#include <vector>
#include <memory>
#include <nodes.h>
#include <opengl.h>
#include <glm/glm.hpp>

class CachedShape {
public:
  CachedShape(State * state);
  void flush(State * state);

  Shape * shape;
  Program * program;
  Material * material;
  glm::mat4 transform;

  std::unique_ptr<GLProgram> glprogram;
  std::unique_ptr<GLUniformBuffer> glcamera;
  std::unique_ptr<GLUniformBuffer> gltransform;
  std::unique_ptr<GLBufferObject> indexbuffer;
  std::unique_ptr<GLBufferObject> vertexbuffer;
  std::unique_ptr<GLVertexAttribute> vertexattrib;
  std::unique_ptr<GLVertexArrayObject> vertexarrayobject;
};

class RenderCache {
public:
  RenderCache();
  ~RenderCache();

  void capture(State * state);

  void compile();
  void flush(State * state);

private:
  class Pimpl;
  std::unique_ptr<Pimpl> self;
};