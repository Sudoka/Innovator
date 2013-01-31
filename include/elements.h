#pragma once

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glstate.h>

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
  GLuint program;
};

class BufferElement {
public:
  class Scope {
  public:
    Scope(BufferState * state)
      : state(state) {
        glBindBuffer(state->target, state->buffer);
    }
    ~Scope() {
      glBindBuffer(state->target, 0);
    }
    BufferState * state;
  };
  BufferState state;
};