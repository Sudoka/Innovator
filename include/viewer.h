#pragma once

#include <memory>
#include <canvas.h>

class Node;

class Viewer : public Canvas {
public:
  Viewer(int width, int height);
  ~Viewer();

  void setSceneGraph(std::shared_ptr<Node> root);
  virtual void renderGL();

  void mouseMoved(int x, int y);
  void mouseButton(int button, int action);
  void keyboard(int button);
  
private:
  class ViewerP;
  std::unique_ptr<ViewerP> self;

};