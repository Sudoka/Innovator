#pragma once

#include <elements.h>
#include <glm/glm.hpp>

class DrawCall;
class GLProgram;
class GLMaterial;

class State {
public:
  State();
  ~State();

  DrawCall * drawcall;
  GLProgram * program;
  GLMaterial * material;
  glm::mat4 transform;
  glm::mat4 viewmatrix;
  glm::mat4 projmatrix;
  CacheElement cacheelem;
  VertexElement vertexelem;

private:
  void push();
  void pop();
  friend class StateScope;

  class StateP;
  std::unique_ptr<StateP> self;
};

class StateScope {
public:
  StateScope(State * state) : state(state) { state->push(); }
  ~StateScope() { state->pop(); }
private:
  State * state;
};