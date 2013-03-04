#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <node.h>

class State;
class GLBufferObject;
class GLVertexAttribute;

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

class AttributeElement : public Bindable {
public:
  AttributeElement();
  ~AttributeElement();

  void push(Bindable * bindable);
  void push(GLBufferObject * buffer);
  void push(GLVertexAttribute * attrib);

  unsigned int indexcount;
  unsigned int attribcount;
  unsigned int instancecount;

private:
  friend class Bindable;
  virtual void bind();
  virtual void unbind();

private:
  GLBufferObject * arraybuffer;
  std::vector<Bindable *> statevec;
};
