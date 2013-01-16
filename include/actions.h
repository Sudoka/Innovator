#pragma once

#include <nodes.h>
#include <memory>
class State;

class RenderAction {
public:
  RenderAction();
  ~RenderAction();

  void apply(std::shared_ptr<Node> node);
  std::unique_ptr<State> state;
};