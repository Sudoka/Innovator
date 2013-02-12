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
  virtual void apply(std::shared_ptr<Node> node) = 0;
  std::unique_ptr<State> state;
};

class RenderAction : public Action {
public:
  RenderAction();
  ~RenderAction();

  void resize(int width, int height);
  void apply(std::shared_ptr<Node> node);

private:
  int width, height;
};

class BoundingBoxAction : public Action {
public:
  BoundingBoxAction();
  ~BoundingBoxAction();
  void apply(std::shared_ptr<Node> node);
  void extendBy(const box3 & box);
  const box3 & getBoundingBox() const;

private:
  box3 box;
};