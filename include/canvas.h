#pragma once

#include <memory>

class Canvas {
public:
  Canvas(int width, int height);
  ~Canvas();

  virtual void renderGL() = 0;
  void loop();

  virtual void mouseMoved(int x, int y) = 0;
  virtual void mouseButton(int button, int action) = 0;
};