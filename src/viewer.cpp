#include <viewer.h>
#include <GL/glfw.h>

#include <nodes.h>
#include <state.h>
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
      renderaction(new RenderAction)
  {}
  ~ViewerP() {}

  float prev_x;
  float prev_y;
  int button;
  bool redraw;
  bool mousedown;
  shared_ptr<Separator> root;
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
  self->renderaction->state->camera->perspective(45, float(width) / float(height), 0.1f, 10000);
  self->renderaction->state->viewport->origin.value = ivec2(0);
  self->renderaction->state->viewport->size.value = ivec2(width, height);
  self->redraw = true;
}

void
Viewer::renderGL()
{
  self->renderaction->apply(self->root.get());
  self->redraw = false;
}

void
Viewer::setSceneGraph(const shared_ptr<Separator> & root)
{
  self->root = root;
  self->renderaction->state->camera->viewAll(root.get());
}

void 
Viewer::mouseMoved(int x, int y)
{
  if (self->mousedown) {
    float dx = (self->prev_x - x);
    float dy = (self->prev_y - y);

    switch (self->button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      self->renderaction->state->camera->orbit(vec2(dx, dy));
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      self->renderaction->state->camera->pan(vec2(dx / 10.0, -dy / 10.0f));
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      self->renderaction->state->camera->zoom(-dy / 10.0f);
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
