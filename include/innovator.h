#pragma once
#include <memory>

class Innovator {
public:
  Innovator(int width, int height);
  ~Innovator();

  void loop();

private:
  class InnovatorP;
  std::unique_ptr<InnovatorP> self;
};