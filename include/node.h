#pragma once

#include <vector>
#include <glstate.h>

class Field;
class RenderAction;
class BoundingBoxAction;

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
