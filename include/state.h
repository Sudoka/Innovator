#pragma once

#include <elements.h>
#include <glm/glm.hpp>

class Camera;
class GLCamera;
class GLProgram;
class GLMaterial;

class State {
public:
  State();
  ~State();

  GLProgram * program;
  GLMaterial * material;
  glm::mat4 transform;
  CacheElement cacheelem;
  VertexElement vertexelem;

  std::unique_ptr<GLCamera> camera;

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