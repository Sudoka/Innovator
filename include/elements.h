#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <opengl.h>
#include <fields.h>
#include <memory.h>

class State;
class Buffer;
class Uniform;
class UniformMatrix4f;
class VertexAttribute;
class GLVertexArrayObject;

class UniformElement {
public:
  void flush(State * state);
  void add(Uniform * uniform);
private:
  std::vector<Uniform*> uniforms;
};

class TransformElement {
public:
  TransformElement();
  ~TransformElement();
  void mult(const glm::mat4 & mat);
  void flush(State * state);
  glm::mat4 matrix;
};

class VertexElement {
public:
  VertexElement();
  ~VertexElement();

  void set(Buffer * buffer);
  void set(VertexAttribute * attrib);

  GLVertexArrayObject * createVAO();

  Buffer * arraybuffer;
  Buffer * vertexbuffer;
  Buffer * elementbuffer;
  Buffer * instancebuffer;
private:
  std::vector<Bindable *> statevec;
};
