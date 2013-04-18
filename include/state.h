#pragma once

#include <memory>
#include <elements.h>

class Draw;
class Camera;
class Program;
class Viewport;
class FeedbackBuffer;

class State {
public:
  class Push {
  public:
    Push(State * state) : state(state) { state->push(); }
    ~Push() { state->pop(); }
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
  FeedbackBuffer * feedbackbuffer;

  VertexElement vertexelem;
  UniformElement uniformelem;
  TransformElement transformelem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};
