#pragma once

#include <memory>
#include <elements.h>

class Draw;
class GLProgram;
class GLQueryObject;
class GLTransformFeedback;

class State {
public:
  State();
  ~State();

  void push();
  void pop();
  void flush(Draw * draw);

  GLProgram * program;
  GLQueryObject * query;
  GLTransformFeedback * feedback;
  VertexElement vertexelem;
  MatrixElement viewmatrixelem;
  MatrixElement modelmatrixelem;
  MatrixElement projmatrixelem;
  ViewportElement viewportelem;
  Uniform3fElement uniform3felem;

private:
  class StateP;
  std::unique_ptr<StateP> self;
};
