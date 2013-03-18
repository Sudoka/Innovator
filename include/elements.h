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
class VertexAttribute;
class GLVertexArrayObject;

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

  void set(Buffer * buffer);
  void set(VertexAttribute * attrib);

  Buffer * getVertexBuffer() const;
  Buffer * getElementBuffer() const;
  Buffer * getInstanceBuffer() const;

  GLVertexArrayObject * createVAO();

private:
  virtual void bind();
  virtual void unbind();

private:
  Buffer * arraybuffer;
  Buffer * vertexbuffer;
  Buffer * elementbuffer;
  Buffer * instancebuffer;
  std::vector<Bindable *> statevec;
};
