#pragma once

#include <memory>
#include <elements.h>

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
  void flush();

  ProgramElement programelem;
  MatrixElement viewmatrixelem;
  MatrixElement modelmatrixelem;
  MatrixElement projmatrixelem;
  BufferElement bufferelem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};