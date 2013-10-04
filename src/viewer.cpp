#include <viewer.h>
#include <GLFW/glfw3.h>

#include <nodes.h>
#include <state.h>
#include <actions.h>

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace std;

class Viewer::ViewerP {
public:
  ViewerP() 
    : prev_x(0),
      prev_y(0),
      button(0),
      redraw(true),
      mousedown(false),
      camera(new Camera),
      root(new Group),
      renderaction(new RenderAction)
  {}
  ~ViewerP() {}

  float prev_x;
  float prev_y;
  int button;
  bool redraw;
  bool mousedown;
  shared_ptr<Group> root;
  shared_ptr<Camera> camera;
  unique_ptr<RenderAction> renderaction;
};

Viewer::Viewer(int width, int height)
  : self(new ViewerP)
{
  this->resize(width, height);
}

Viewer::~Viewer()
{

}

void
Viewer::scheduleRedraw()
{
  self->redraw = true;
}

bool
Viewer::needRedraw() const
{
  return self->redraw;
}

void
Viewer::resize(int width, int height)
{
  self->camera->aspectRatio.value = float(width) / float(height);
  self->renderaction->setViewport(ivec4(0, 0, width, height));
  self->redraw = true;
}

void
Viewer::render()
{
  self->renderaction->apply(self->root.get());
  self->redraw = false;
}

void
Viewer::setSceneGraph(const shared_ptr<Separator> & root)
{
  self->root->children.values.push_back(self->camera);
  self->root->children.values.push_back(root);
  self->camera->viewAll(root.get());
}

void 
Viewer::mouseMoved(int x, int y)
{
  if (self->mousedown) {
    float dx = (self->prev_x - x);
    float dy = (self->prev_y - y);

    switch (self->button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      self->camera->orbit(vec2(dx, dy));
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      self->camera->pan(vec2(dx / 10.0, -dy / 10.0f));
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      self->camera->zoom(dy / 10.0f);
      break;
    }
    self->redraw = true;
  }
  self->prev_x = (float)x;
  self->prev_y = (float)y;
}

void
Viewer::mouseButton(int button, int action)
{
  self->button = button;
  self->mousedown = action == GLFW_PRESS;
}

void
Viewer::keyboard(int button)
{

}
