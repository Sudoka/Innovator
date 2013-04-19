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
  ViewerP(int width, int height) 
    : prev_x(0),
      prev_y(0),
      button(0),
      redraw(true),
      mousedown(false),
      camera(new Camera),
      viewport(new Viewport),
      renderaction(new RenderAction)
  {}
  ~ViewerP() {}

  float prev_x;
  float prev_y;
  int button;
  bool redraw;
  bool mousedown;
  Separator::ptr root;
  Camera::ptr camera;
  Viewport::ptr viewport;
  unique_ptr<RenderAction> renderaction;
};

Viewer::Viewer(int width, int height)
  : self(new ViewerP(width, height))
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
  self->camera->perspective(45, float(width) / float(height), 0.1f, 10000);
  self->viewport->origin.value = ivec2(0);
  self->viewport->size.value = ivec2(width, height);
}

void
Viewer::renderGL()
{
  self->renderaction->apply(self->root);
  self->redraw = false;
}

void
Viewer::setSceneGraph(Separator::ptr root)
{
  self->root.reset(new Separator);
  self->root->children.values.push_back(self->camera);
  self->root->children.values.push_back(self->viewport);
  self->root->children.values.push_back(root);
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
      self->camera->orbit(vec2(dx * 10.0, dy * 10.0));
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
