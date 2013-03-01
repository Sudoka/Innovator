#pragma once

#include <vector>

class Field;
class RenderAction;
class BoundingBoxAction;

class Bindable {
public:
  virtual void bind() = 0;
  virtual void unbind() = 0;
};

class BindScope {
public:
  BindScope(Bindable * b) : bindable(b) {
    this->bindable->bind();
  }
  ~BindScope() {
    this->bindable->unbind();
  }
private:
  Bindable * bindable;
};

class FieldContainer {
protected:
  std::vector<Field*> fields;
};

class Node : public Bindable, public FieldContainer {
public:
  virtual ~Node() {}
  virtual void traverse(RenderAction * action) {}
  virtual void traverse(BoundingBoxAction * action) {}

  virtual void bind() {}
  virtual void unbind() {}

  typedef std::shared_ptr<Node> ptr;
};
