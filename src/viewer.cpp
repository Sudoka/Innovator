#include <viewer.h>
#include <GL/glfw.h>

#include <nodes.h>
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
  self->camera->perspective(45, float(width) / float(height), 0.1f, 100);
  self->renderaction->resize(width, height);
}

void
Viewer::renderGL()
{
  self->renderaction->apply(self->root);
  self->redraw = false;
}

void
Viewer::setSceneGraph(std::shared_ptr<Node> root)
{
  self->root.reset(new Group);
  self->root->addChild(self->camera);
  self->root->addChild(root);
  self->camera->viewAll(root);
}

void 
Viewer::mouseMoved(int x, int y)
{
  if (self->mousedown) {
    float dx = (self->prev_x - x) / 100.0f;
    float dy = (self->prev_y - y) / 100.0f;

    switch (self->button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      self->camera->orbit(vec2(dx * 100.0, dy * 100.0));
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      self->camera->pan(vec2(dx, -dy));
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      self->camera->zoom(-dy);
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
