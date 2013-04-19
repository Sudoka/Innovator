#pragma once

#include <memory>
#include <elements.h>

class Draw;
class Camera;
class Program;
class Viewport;

class State {
public:
  class Scope {
  public:
    Scope(std::unique_ptr<State> & state) : state(state.get()) { state->push(); }
    ~Scope() { state->pop(); }
  private:
    State * state;
  };
  State();
  ~State();

  void push();
  void pop();
  void flush(Draw * draw);

  Camera * camera;
  Program * program;
  Viewport * viewport;

  VertexElement vertexelem;
  UniformElement uniformelem;
  TextureElement textureelem;
  TransformElement transformelem;
  TransformFeedbackElement feedbackelem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};
