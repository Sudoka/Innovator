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
      mousedown(false),
      camera(new Camera),
      renderaction(new RenderAction)  
  {}
  ~ViewerP() {}

  int prev_x;
  int prev_y;
  bool mousedown;
  shared_ptr<Group> root;
  shared_ptr<Camera> camera;
  shared_ptr<RenderAction> renderaction;
};

Viewer::Viewer(int width, int height)
  : Canvas(width, height),
    self(new ViewerP)
{
  self->camera->perspective(45, 640.0f / 480.0f, 0.1f, 100);
  self->camera->moveTo(vec3(0.0, 0.0, 5.0));
}

Viewer::~Viewer()
{

}

void
Viewer::renderGL()
{
  self->renderaction->apply(self->root);
}

void
Viewer::setSceneGraph(std::shared_ptr<Node> root)
{
  self->root.reset(new Group);
  self->root->addChild(self->camera);
  self->root->addChild(root);
}

void 
Viewer::mouseMoved(int x, int y)
{
  if (self->mousedown) {
    int dx = self->prev_x - x;
    int dy = self->prev_y - y;
    cout << "mouse dragged: <dx=" << dx << "dy=" << dy << ">" << endl;
    self->camera->orbit(vec2(dx, dy));
  }
  self->prev_x = x;
  self->prev_y = y;
}

void
Viewer::mouseButton(int button, int action)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    self->mousedown = action == GLFW_PRESS;
  }
}

void
Viewer::keyboard(int button)
{

}
