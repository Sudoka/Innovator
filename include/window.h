#pragma once

#include <string>
#include <memory>

class Window {
public:
  Window(int width, int height, const std::string & title);
  ~Window();

  virtual void render();

private:
  virtual void renderScene() = 0;

private:
  class Pimpl;
  std::unique_ptr<Pimpl> self;
};