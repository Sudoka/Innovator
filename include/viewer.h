#pragma once

#include <memory>
#include <canvas.h>

class Node;

class Viewer {
public:
  Viewer(int width, int height);
  ~Viewer();

  void setSceneGraph(std::shared_ptr<Node> root);
  void renderGL();

  void mouseMoved(int x, int y);
  void mouseButton(int button, int action);
  void keyboard(int button);

  bool needRedraw() const;
  
private:
  class ViewerP;
  std::unique_ptr<ViewerP> self;
};