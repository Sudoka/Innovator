#pragma once

#include <nodes.h>
#include <memory>
#include <box3.h>

class State;
class box3;

class Action {
public:
  Action();
  ~Action();
  virtual void apply(Node * node) = 0;
  std::unique_ptr<State> state;
};

class RenderAction : public Action {
public:
  RenderAction();
  ~RenderAction();
  void apply(Node * node);
  void setViewport(const glm::ivec4 & viewport);
private:
  glm::ivec4 viewport;
};

class BoundingBoxAction : public Action {
public:
  BoundingBoxAction();
  ~BoundingBoxAction();
  void apply(Node * node);
  void extendBy(const box3 & box);
  const box3 & getBoundingBox() const;
  static box3 getBoundingBox(Separator * root);

private:
  box3 box;
};
