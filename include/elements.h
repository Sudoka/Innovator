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

  VertexAttribute * get(unsigned int index);

  unsigned int getIndexCount() const;
  unsigned int getVertexCount() const;
  unsigned int getInstanceCount() const;

private:
  virtual void bind();
  virtual void unbind();

private:
  unsigned int vertexcount;
  unsigned int indexcount;
  unsigned int instancecount;

  ArrayBuffer * arraybuffer;
  ElementBuffer * elementbuffer;
  std::vector<Bindable *> statevec;
  std::vector<VertexAttribute *> attributes;
};
