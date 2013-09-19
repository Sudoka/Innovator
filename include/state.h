#pragma once

#include <elements.h>
#include <glm/glm.hpp>

class Shape;
class Program;
class Material;
class RenderCache;

class State {
public:
  State();
  ~State();

  Shape * shape;
  Program * program;
  Material * material;
  RenderCache * rendercache;
  glm::mat4 viewmatrix;
  glm::mat4 projmatrix;
  glm::mat4 transform;

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