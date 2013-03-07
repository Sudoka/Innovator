#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <opengl.h>
#include <fields.h>

class State;
class VertexAttribute;

template <typename T>
class Buffer;

class MatrixElement {
public:
  void updateGL(State * state);
  std::string name;
  glm::mat4 matrix;
};

class ViewportElement {
public:
  ViewportElement();
  void updateGL(State * state);
  glm::ivec2 size;
  glm::ivec2 origin;
};

class VertexElement : public Bindable {
public:
  VertexElement();
  ~VertexElement();

  void set(Buffer<int> * buffer);
  void set(Buffer<float> * buffer);
  void set(VertexAttribute * attrib);

  Buffer<int> * getElementBuffer() const;
  Buffer<float> * getVertexBuffer() const;
  Buffer<float> * getInstanceBuffer() const;

private:
  virtual void bind();
  virtual void unbind();

private:
  Buffer<int> * elementbuffer;
  Buffer<float> * arraybuffer;
  Buffer<float> * vertexbuffer;
  Buffer<float> * instancebuffer;
  std::vector<Bindable *> statevec;
};
