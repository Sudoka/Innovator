#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <opengl.h>
#include <fields.h>

class State;
class Buffer;
class ArrayBuffer;
class ElementBuffer;
class IntBuffer;
class FloatBuffer;
class VertexAttribute;

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

  void set(ArrayBuffer * buffer);
  void set(ElementBuffer * buffer);
  void set(VertexAttribute * attrib);

  ArrayBuffer * getVertexBuffer() const;
  ArrayBuffer * getInstanceBuffer() const;
  ElementBuffer * getElementBuffer() const;

private:
  virtual void bind();
  virtual void unbind();

private:
  ArrayBuffer * arraybuffer;
  ArrayBuffer * vertexbuffer;
  ArrayBuffer * instancebuffer;
  ElementBuffer * elementbuffer;
  std::vector<Bindable *> statevec;
};
