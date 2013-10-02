#pragma once

#include <vector>
#include <memory>
#include <nodes.h>
#include <opengl.h>
#include <glm/glm.hpp>

class ShapeInfo {
public:
  int shapeindex;
  int programindex;
  int materialindex;
  glm::mat4 transform;
};

struct GLMaterial {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
};

struct GLTransform {
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;
  glm::mat4 ModelMatrix;
};

class GLRenderer {
public:
  virtual void capture(State * state) = 0;
  virtual void compile() = 0;
  virtual void flush(State * state) = 0;
};

class GL3Renderer : public GLRenderer {
public:
  GL3Renderer();
  ~GL3Renderer();

  void capture(State * state);

  void compile();
  void flush(State * state);

private:
  class Pimpl;
  std::unique_ptr<Pimpl> self;
};