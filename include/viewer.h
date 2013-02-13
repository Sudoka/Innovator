#pragma once

#include <memory>
#include <nodes.h>

class Viewer {
public:
  Viewer(int width, int height);
  ~Viewer();

  void setSceneGraph(Node::ptr root);
  void renderGL();

  void resize(int width, int height);
  void mouseMoved(int x, int y);
  void mouseButton(int button, int action);
  void keyboard(int button);

  void scheduleRedraw();
  bool needRedraw() const;
  
private:
  class ViewerP;
  std::unique_ptr<ViewerP> self;
};