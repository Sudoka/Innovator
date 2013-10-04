#pragma once

#include <nodes.h>
#include <memory>

class Viewer {
public:
  Viewer(int width, int height);
  ~Viewer();

  void setSceneGraph(const std::shared_ptr<Separator> & root);
  virtual void render();

  void resize(int width, int height);
  void mouseMoved(int x, int y);
  virtual void mouseButton(int button, int action);
  void keyboard(int button);

  void scheduleRedraw();
  bool needRedraw() const;
  
private:
  class ViewerP;
  std::unique_ptr<ViewerP> self;
};
