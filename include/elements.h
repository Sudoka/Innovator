#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glstate.h>

class State;
class IndexBuffer;
class VertexAttribute;

class MatrixElement {
public:
  void updateGL(State * state);
  std::string name;
  glm::mat4 matrix;
};

class AttributeElement : public Bindable {
public:
  AttributeElement();
  ~AttributeElement();
  void set(VertexAttribute * attribute);
  void set(IndexBuffer * indices);

  IndexBuffer * get() const;
  VertexAttribute * get(const int index) const;

private:
  friend class Bindable;
  virtual void bind();
  virtual void unbind();

private:
  IndexBuffer * indices;
  std::vector<VertexAttribute *> attributes;
};