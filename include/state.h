#pragma once

#include <memory>
#include <elements.h>

class Camera;
class Program;
class Viewport;
class DrawCall;

class State {
public:
  State();
  ~State();

  void flush(DrawCall * drawcall);

  Camera * camera;
  Program * program;
  Viewport * viewport;

  VertexElement vertexelem;
  TextureElement textureelem;
  TransformElement transformelem;

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