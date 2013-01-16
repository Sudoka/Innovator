#pragma once

#include <opengl.h>
#include <string>
#include <glm/glm.hpp>

class State;

class MatrixElement {
public:
  void updateGL(State * state);

  std::string name;
  glm::mat4 matrix;
};

class ProgramElement {
public:
  void updateGL(State * state);
  ShaderProgram program;
};