#include <actions.h>
#include <state.h>
#include <GL/glew.h>

using namespace std;

Action::Action()
  : state(nullptr) {}
Action::~Action() {}

RenderAction::RenderAction()
  : Action(),
    width(-1),
    height(-1)
{}

RenderAction::~RenderAction() {}

void
RenderAction::resize(int width, int height)
{
  this->width = width;
  this->height = height;
}

void
RenderAction::apply(shared_ptr<Node> node)
{
  glViewport(0, 0, this->width, this->height);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  state.reset(new State);
  node->traverse(this);
}

BoundingBoxAction::BoundingBoxAction()
  : Action() {}

BoundingBoxAction::~BoundingBoxAction() {}

void
BoundingBoxAction::extendBy(const box3 & box)
{
  this->box.extendBy(box.min);
  this->box.extendBy(box.max);
}

void
BoundingBoxAction::apply(shared_ptr<Node> node)
{
  state.reset(new State);
  this->box.makeEmpty();
  node->traverse(this);
}

const box3 & 
BoundingBoxAction::getBoundingBox() const
{
  return this->box;
}