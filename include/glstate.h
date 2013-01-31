#pragma once

#include <GL/glew.h>

class BufferState {
public:
  BufferState() : target(GL_INVALID_VALUE), buffer(0) {}
  BufferState(GLenum target, GLuint buffer = 0) : target(target), buffer(buffer) {}
  GLuint buffer;
  GLenum target;
};

