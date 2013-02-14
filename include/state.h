#pragma once

#include <memory>
#include <elements.h>

class Draw;
class Program;

class State {
public:
  State();
  ~State();

  enum Uniforms {
    ViewMatrix,
    ModelMatrix,
    ProjectionMatrix,
    NUM_UNIFORMS
  };

  enum Attributes {
    VertexPosition    = 0,
    VertexNormal      = 1,
    TextureCoordinate = 2,
    NUM_ATTRIBUTES    = 3
  };

  void push();
  void pop();
  void flush(Draw * draw);

  Program * program;
  AttributeElement attribelem;
  MatrixElement viewmatrixelem;
  MatrixElement modelmatrixelem;
  MatrixElement projmatrixelem;
  ViewportElement viewportelem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};
