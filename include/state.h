#pragma once

#include <elements.h>
#include <memory>
#include <functional>

class DrawCall;

class State {
public:
  State();
  ~State();

  DrawElement drawelem;
  CacheElement cacheelem;
  VertexElement vertexelem;
  ProgramElement programelem;
  ViewMatrixElement viewmatelem;
  TransformElement transformelem;
  ProjectionMatrixElement projmatelem;

  std::function<void()> capture();

private:
  void push();
  void pop();
  friend class StateScope;

  class StateP;
  std::unique_ptr<StateP> self;
};

class StateScope {
public:
  StateScope(State * state) : state(state) {
    state->push();
  }
  ~StateScope() {
    state->pop();
  }
private:
  State * state;
};