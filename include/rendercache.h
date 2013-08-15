#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <opengl.h>

class State;
class DrawCall;

class DrawCache {
public:
  DrawCache(State * state);
  void flush();
  std::function<void()> updateProgram() const;
  std::function<void()> updateMaterial() const;
  std::function<void()> executeDrawCall() const;

  State * state;
  DrawCall * drawcall;
  GLProgram * program;
  GLMaterial * material;
  glm::mat4 transform;
};

class RenderCache {
public:
  RenderCache();
  ~RenderCache();

  void compile();
  void flush();

  std::vector<DrawCache> drawlist;
  std::vector<std::function<void()>> glcalls;
};