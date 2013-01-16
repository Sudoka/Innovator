#include <actions.h>
#include <state.h>
#include <GL/glew.h>

using namespace std;

RenderAction::RenderAction()
  : state(nullptr) {}

RenderAction::~RenderAction() {}

void
RenderAction::apply(shared_ptr<Node> node)
{
  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  state.reset(new State);
  node->renderGL(this);
}