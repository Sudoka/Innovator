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