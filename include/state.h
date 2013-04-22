#pragma once

#include <memory>
#include <elements.h>

class Shape;
class Camera;
class Program;
class Viewport;

class State {
public:
  State();
  ~State();

  void push();
  void pop();
  void flush(Shape * shape);

  Camera * camera;
  Program * program;
  Viewport * viewport;

  VertexElement vertexelem;
  TextureElement textureelem;
  TransformElement transformelem;
  TransformFeedbackElement feedbackelem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};
